/* Particle
 * A component that represents a particle that emits rays of light within a predefined FOV.
 * The heading/head of the particle is in the middle of the FOV.
 */
import QtQuick 2.12
import "draw.js" as Draw

Item {
    id: particle

    property var pos: Qt.vector2d(0, 0)     // position of the particle/player inside the canvas
    property var heading: 0                 // the direction of the particle/player (angle in radians)

    property var numRays: 30;               // the amount of light rays emitted by the particle
    property var rays: createListOfRays(numRays)

    property var size: 5                    // particle drawing size
    property var color: "white"             // particle drawing color
    property var headingSize: 25            // drawing size of the head direction

    property bool fisheye: false            // control fisheye rendering
    property var fov: 60                    // field of view: 60º means the focal length of the camera ranges from -30º to 30º


    // show: draws the particle and its rays on the canvas
    function show(ctx) {        
        // draw rays (fov)
        for (let i = 0; i < particle.rays.length; ++i)
            particle.rays[i].show(ctx);

        // draw green arrow that points the diretion of the head
        Draw.line(ctx, particle.pos.x, particle.pos.y,
                  particle.pos.x+Math.cos(heading)*headingSize, particle.pos.y+Math.sin(heading)*headingSize,
                  "lime");

        // draw particle body
        Draw.circle(ctx, particle.pos.x, particle.pos.y, particle.size, particle.color);
    }

    // rotate: called when the head of the particle is turned by keypresses
    function rotate(angle) {
        particle.heading += angle;

        const startAngle = -particle.fov/2;
        const incAngle =  particle.fov/numRays;

        //console.log("Particle.rotate angle=" + angle + " particle.heading=" + particle.heading + " startAngle=" + startAngle + " incAngle=" + incAngle);

        let a = startAngle;
        for (let i = 0; i < particle.rays.length; ++i) {
            particle.rays[i].setAngle(deg2rad(a) + particle.heading);
            a += incAngle;
        }
    }

    // move: called when the particle is moved forward or backwards by keypresses
    function move(inc) {
        //console.log("Particle.move inc=" + inc)
        let velocity = fromAngle(particle.heading);

        // adjust the magnitude of velocity based on the increment amount
        let newMag = inc;
        let curMag = Math.sqrt((velocity.x * velocity.x) + (velocity.y * velocity.y));
        velocity.x = velocity.x * newMag / curMag;
        velocity.y = velocity.y * newMag / curMag;

        // add velocity to the current position of the particle
        particle.pos = particle.pos.plus(velocity);
    }

    // fromAngle: creates a vector2d equivalent to the input angle (radians)
    function fromAngle(rad) {
        return Qt.vector2d(Math.cos(rad), Math.sin(rad));
    }

    // deg2rad: converts degrees to radians
    function deg2rad(degrees) {
        return degrees * (Math.PI / 180.0);
    }

    // rad2deg: converts degrees to radians
    function rad2deg(radians) {
        return radians * (180.0 / Math.PI);
    }

    // createListOfRays: returns a list of QML components of type Ray
    function createListOfRays(size) {
        var component = Qt.createComponent("Ray.qml");
        if (component === null) {
            // Error Handling
            console.log("Particle !!! error creating component");
            console.log(component.errorString());
            return;
        }

        var list = [];
        const startAngle = -particle.fov/2;
        const endAngle = particle.fov/2;
        const incAngle =  particle.fov / particle.numRays;

        // create N rays within the FOV
        for (let i = startAngle, x = 0; i < endAngle; i += incAngle, ++x) {

            const radians = deg2rad(i);
            //console.log("Particle: i=" + i + " radians=" + radians);

            // invoke the constructor of Boundary with the following parameters (property binding is used for Ray.pos)
            var options = {
                "pos": Qt.binding(function(){ return particle.pos; }),
                "angle": radians
            };
            var dynamicRay = component.createObject(particle, options);

            if (dynamicRay === null) {
                console.log("Particle !!! error creating object");
                console.log(component.errorString());
                return;
            }

            list[x] = dynamicRay;
        }

        //console.log("Particle: createListOfRays list sz=" + list.length);
        return list;
    }

    // look: test each ray of the particle for a collision with the closest wall
    function look(walls, ctx) {
        // scene: record all the distance values where an ray touches a wall: 40 rays result in 40 distance values
        let scene = [];

        // draw rays into the direction of the closest wall
        for (let i = 0; i < particle.rays.length; ++i) {
            const ray = particle.rays[i];
            let closestIntersectionPt = null;
            let recordDist = Number.MAX_VALUE; // Infinity

            for (let j = 0; j < walls.length; ++j) {
                // figure out if particle.rays[i] hits the wall walls[j]
                const wall = walls[j];
                const pt = ray.cast(wall);

                // check if there is an intersection point between the ray and this wall
                if (pt) {
                    /* decrease fisheye effect:
                     * calculate the distance projected (PJ) on the camera direction as the Euclidean Distance (ED) creates the fisheye effect
                     *            `.
                     *             .`x   w
                     *        ED .'|  `.   a
                     *        .'   |    `.    l
                     *      O......|......`.    l
                     *         PJ           `.
                     *
                     */

                    // euclidian distance
                    let dist = distance(particle.pos, pt);

                    // decrease fisheye effect
                    if (!fisheye) {
                        // calculate the angle diference between the ray and the "camera"
                        let a = particle.heading - ray.heading;
                        if (a < 0)
                            a += 2*Math.PI;
                        if (a > 2*Math.PI)
                            a -= 2*Math.PI;

                        // multiply the cosine of the new angle to decrease the effect
                        dist *= Math.cos(a);
                    }

                    if (dist < recordDist) {
                        recordDist = dist;
                        closestIntersectionPt = pt;
                    }
                }
            }

            // save the closest distance to any wall
            scene[i] = recordDist;

            // draw a line from the particle position to the intersection point
            if (closestIntersectionPt) {
               Draw.line(ctx, particle.pos.x, particle.pos.y, closestIntersectionPt.x, closestIntersectionPt.y, "yellow");
            }
        }

        return scene;
    }

    /* distance: calculates the Euclidian distance between 2 points
     *
     *           . b
     *         .`|
     *       .´  |
     *     .´    | opp
     * a .´______|
     *       adj
     */
    function distance(a, b) {
        var horizDist = Math.abs(a.x - b.x); // adjacent
        var vertDist = Math.abs(a.y - b.y);  // opposite

        // hyp^2 = opp^2 + adj^2
        return Math.sqrt(horizDist*horizDist + vertDist*vertDist);
    }

    // update: updates the particle position in the world
    function update(x, y) {
        particle.pos.x = x;
        particle.pos.y = y;
    }
}

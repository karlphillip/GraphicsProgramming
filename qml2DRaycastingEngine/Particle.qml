/* Particle
 * A component that represents a particle that emits rays of light within a predefined FOV.
 * The heading/head of the particle is in the middle of the FOV.
 */
import QtQuick 2.12
import "draw.js" as Draw

Item {
    id: particle

    property var pos: Qt.vector2d(0, 0)     // starting position of the particle inside the canvas
    property var heading: 0                 // the current direction of the particle (angle in radians)

    property var numRays: 30;               // the amount of light rays that this particle emits
    property var rays: createListOfRays(numRays)

    property var size: 5                    // particle drawing size
    property var color: "white"             // particle drawing color

    property bool fisheye: false            // control fisheye rendering
    property var fov: 90                    // field of view 60º means that the particle vision ranges from -30º to 30º


    // show: draws the particle and its rays on the canvas
    function show(ctx) {        
        for (let i = 0; i < particle.rays.length; ++i)
            particle.rays[i].show(ctx);

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
            particle.rays[i].setAngle(degToRad(a) + particle.heading);
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

    // degToRad: converts degrees to radians
    function degToRad(degrees) {
        return degrees * (Math.PI / 180);
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
        const incAngle =  particle.fov / size;

        // create N rays within the FOV
        for (let i = startAngle, x = 0; i < endAngle; i += incAngle, ++x) {

            const radians = degToRad(i)
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
        // scene: record all the distance values where an ray touches a wall. 40 rays give 40 distance values.
        let scene = [];

        // draw rays into the direction of the closest wall
        for (let i = 0; i < particle.rays.length; ++i) {
            const ray = particle.rays[i];
            let closestIntersectionPt = null;
            let recordDist = Number.MAX_VALUE; // Infinity

            for (let j = 0; j < walls.length; ++j) {
                // figure out if this ray of light hits the wall
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

                    // angle of the ray relative to the direction of the camera
                    const a = ray.heading - particle.heading;

                    // decrease fisheye effect
                    if (!fisheye) {
                        dist *= Math.cos(a);
                    }

                    if (dist < recordDist) {
                        recordDist = dist;
                        closestIntersectionPt = pt;
                    }
                }
            }

            if (closestIntersectionPt) {
               Draw.line(ctx, particle.pos.x, particle.pos.y, closestIntersectionPt.x, closestIntersectionPt.y, "yellow");
            }

            // detect the cloest distance and keep it in this array
            scene[i] = recordDist;
        }

        return scene;
    }

    // distance: calculates the Euclidian distance between 2 points
    function distance(a, b) {
        var horizDist = a.x - b.x;
        var vertDist = a.y - b.y;
        return Math.sqrt(horizDist*horizDist + vertDist*vertDist);
    }

    // update: updates the particle position in the world
    function update(x, y) {
        particle.pos.x = x;
        particle.pos.y = y;
    }
}

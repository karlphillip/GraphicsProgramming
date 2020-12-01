/* Particle
 * A component that represents a particle that emits light in all directions.
 */
import QtQuick 2.12
import "draw.js" as Draw

Item {
    id: particle

    property var pos: Qt.vector2d(0, 0)
    property var rays: createListOfRays()
    property var numRays: 30; // the amount of light rays that this particle emits

    function show(ctx) {
        Draw.circle(ctx, pos.x, pos.y, 4, "white");

        for (let i = 0; i < rays.length; ++i)
            rays[i].show(ctx);
    }

    // createListOfRays: returns a list of QML components of type Ray
    function createListOfRays() {
        var list = new Array(numRays);
        const incSize =  360 / numRays;

        for (let i = 0, x = 0; i < 360; i += incSize, ++x) {
            var component = Qt.createComponent("Ray.qml");
            if (component === null) {
                // Error Handling
                console.log("Particle !!! error creating component");
                console.log(component.errorString());
                return;
            }

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

    function degToRad(degrees) {
        return degrees * (Math.PI / 180);
    }

    // lookWall: test each ray of the particle for a collision with a single wall (helpful for debugging)
    function lookWall(wall, ctx) {
        // draw rays into the direction of the wall
        for (let i = 0; i < rays.length; ++i) {
            const pt = rays[i].cast(wall);

            // check if the point of intersection exists
            if (pt)
                Draw.line(ctx, particle.pos.x, particle.pos.y, pt.x, pt.y, "yellow");
        }
    }

    // look: test each ray of the particle for a collision with the closest wall
    function look(walls, ctx) {
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
                    const dist = distance(particle.pos, pt);
                    if (dist < recordDist) {
                        recordDist = dist;
                        closestIntersectionPt = pt;
                    }
                }
            }

            if (closestIntersectionPt)
                Draw.line(ctx, particle.pos.x, particle.pos.y, closestIntersectionPt.x, closestIntersectionPt.y, "yellow");
        }
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

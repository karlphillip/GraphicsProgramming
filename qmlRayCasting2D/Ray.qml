/* Ray
 * A component that represents a ray of light.
 * It calculates the point of interception with a wall if they collide.
 */
import QtQuick 2.0
import "draw.js" as Draw

Item {
    id: ray

    property var angle: 0.0                     // the angle of the light in radians
    property vector2d pos: Qt.vector2d(0, 0)    // starting position of the ray
    property vector2d dir: fromAngle(angle)     // unit vector that guides the direction of the light

    property var raySize: 8                     // size of the ray for debugging purposes

    // show: draw the light of ray on the screen (red)
    function show(ctx) {
        Draw.line(ctx, ray.pos.x, ray.pos.y, ray.pos.x+ray.dir.x*raySize, ray.pos.y+ray.dir.y*raySize, "red");
        //console.log("Ray.show: pos=" + pos + " angle=" + angle + " dir=" + dir);
    }

    // fromAngle: creates a unit vector with the direction of the angle (radians)
    function fromAngle(rad) {
        return Qt.vector2d(Math.cos(rad), Math.sin(rad));
    }

    // lookAt: sets the direction to where the ray is looking at
    function lookAt(x, y) {
        ray.dir.x = x - ray.pos.x;
        ray.dir.y = y - ray.pos.y;
        ray.dir = ray.dir.normalized(); // shortens the ray
    }

    // cast: returns the point of intersection between two lines. Otherwise, it returns NULL.
    function cast(wall) {
        /* The intersection point falls within the first line segment if 0.0 <= t <= 1.0
         * AND if it falls within the second line segment if:  u >= 0
         */
        // wall endpoints
        var x1 = wall.a.x;
        var y1 = wall.a.y;
        var x2 = wall.b.x;
        var y2 = wall.b.y;
        //console.log("Ray::cast wall pt1=" + x1 + "," + y1 + " pt2=" + x2 + "," + y2);

        // ray endpoints
        var x3 = ray.pos.x;
        var y3 = ray.pos.y;
        var x4 = ray.pos.x + ray.dir.x;
        var y4 = ray.pos.y + ray.dir.y;
        //console.log("Ray::cast ray pt1=" + x3 + "," + y3 + " pt2=" + x4 + "," + y4);

        // calculate the denominator
        var den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        //console.log("den=" + den);

        // if den is 0, it means that ray and wall are perfectly parallel to each other and will never TouchPoint
        if (den === 0)
            return {};

        // calculate t and u
        var t =  ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
        var u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
        //console.log("t=" + t);
        //console.log("u=" + u);

        // if they intersect, calculate the point of intersection
        if (t > 0 && t < 1 && u > 0) {
            var pt = Qt.vector2d(0, 0);
            pt.x = x1 + t * (x2 - x1);
            pt.y = y1 + t * (y2 - y1);
            return pt;
        }

        // otherwise, return undefined: no intersection found
        return {};
    }

    Component.onCompleted: {
        console.log("Ray constructed: pos=" + pos.x + "," + pos.y + " angle=" + angle + " dir=" + dir);
    }
}

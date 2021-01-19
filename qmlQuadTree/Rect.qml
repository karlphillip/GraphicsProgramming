/*
 * Rect is an element that stores the x,y,width,height position of a rectangle on the screen.
 */
import QtQuick 2.12

Item {
    id: rect

    property var w : 0
    property var h : 0

    // contains: check if a point is within the boundary of this Rect
    function contains(point) {
        return (point.x >= this.x - this.w &&
                point.x <= this.x + this.w &&
                point.y >= this.y - this.h &&
                point.y <= this.y + this.h);
    }

    // intersects: AABB collision detection
    function intersects(range) {
        return !(range.x - range.w > this.x + this.w ||
                 range.x + range.w < this.x - this.w ||
                 range.y - range.h > this.y + this.h ||
                 range.y + range.h < this.y - this.h);
    }
}

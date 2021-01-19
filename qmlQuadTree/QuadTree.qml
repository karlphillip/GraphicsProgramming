/*
 * QuadTree is an element that stores particles on the screen as 2D points and subdivides
 * itself into 4 other QuadTrees when the amount of particles stored hit maximum capacity.
 *
 *  ---------------
 * | .     | ..| . |
 * |    .  |---|---|
 * | .     |  .| . |
 * |---------------|
 * |     . |     . |
 * |       |  . .  |
 * | .     |     . |
 *  ---------------
 *
 * This is a great way to structure data and provide a fast searching aproach: n log(n)
 */
import QtQuick 2.0
import "draw.js" as Draw

Item {
    id: quadTree

    property Rect boundary
    property var points: []
    property int capacity: 0    // how big is the QuadTree
    property var alreadyDivided: false

    // define 4 subsections as QuadTrees
    property var ne
    property var nw
    property var se
    property var sw

    /* insert: returns true when it successfully adds a point to the QuadTree
     * and subdivides it into other 4 QuadTrees when the maximum capacity is hit.
     */
    function insert(point) {
        // check if the point is within the boundary of this QuadTree. Otherwise, do nothing!
        if (!this.boundary.contains(point)) {
            //console.log("QuadTree.insert(): point is outside the boundary");
            return false;
        }

        if (points.length < capacity) {
            points.push(point);
            return true;
        }

        // the amount of points stored (i.e. capacity) defines when the QuadTree needs to subdivide
        if (alreadyDivided === false)
            subdivide();

        // insert the point into subsections
        if (this.ne.insert(point))
            return true;

        if (this.nw.insert(point))
            return true;

        if (this.se.insert(point))
            return true;

        if (this.sw.insert(point))
            return true;

        return false;
    }

    /* subdivide: divides the Rect into 4 sections
     *
     *   NW | NE
     *   ---|---
     *   SW | SE
     */
    function subdivide() {
        /* in order to subdivide, identify the 4 rectangular areas for the new sections */

        var rectComponent = Qt.createComponent("Rect.qml");
        if (rectComponent === null) {
            console.log("!!! error creating rectComponent");
            console.log(component.errorString());
            return;
        }

        let rectNE = rectComponent.createObject(quadTree, {
            "x": this.boundary.x + this.boundary.w / 2,
            "y": this.boundary.y - this.boundary.h / 2,
            "w": this.boundary.w / 2,
            "h": this.boundary.h / 2
        });

        let rectNW = rectComponent.createObject(quadTree, {
            "x": this.boundary.x - this.boundary.w / 2,
            "y": this.boundary.y - this.boundary.h / 2,
            "w": this.boundary.w / 2,
            "h": this.boundary.h / 2
        });

        let rectSE = rectComponent.createObject(quadTree, {
            "x": this.boundary.x + this.boundary.w / 2,
            "y": this.boundary.y + this.boundary.h / 2,
            "w": this.boundary.w / 2,
            "h": this.boundary.h / 2
        });

        let rectSW = rectComponent.createObject(quadTree, {
            "x": this.boundary.x - this.boundary.w / 2,
            "y": this.boundary.y + this.boundary.h / 2,
            "w": this.boundary.w / 2,
            "h": this.boundary.h / 2
        });

        /* allocate the 4 new QuadTrees */

        var qtComponent = Qt.createComponent("QuadTree.qml");
        if (qtComponent === null) {
            console.log("!!! error creating qtComponent");
            console.log(component.errorString());
            return;
        }

        nw = qtComponent.createObject(quadTree, { "boundary": rectNW, "capacity": this.capacity });
        ne = qtComponent.createObject(quadTree, { "boundary": rectNE, "capacity": this.capacity });
        sw = qtComponent.createObject(quadTree, { "boundary": rectSW, "capacity": this.capacity });
        se = qtComponent.createObject(quadTree, { "boundary": rectSE, "capacity": this.capacity });

        alreadyDivided = true;
    }

    // show: draw the QuadTree with its subsections and points
    function show(ctx) {
        Draw.rectCenter(ctx, this.boundary.x, this.boundary.y, this.boundary.w*2, this.boundary.h*2, "black", "white");

        if (this.alreadyDivided) {
            this.ne.show(ctx);
            this.nw.show(ctx);
            this.se.show(ctx);
            this.sw.show(ctx);
        }

        for (let i = 0; i < this.points.length; ++i) {
            Draw.circle(ctx, this.points[i].x, this.points[i].y, 2, "white");
        }
    }

    // query: returns how many points are within a rectangular area
    function query(rectArea, pointsFound) {
        // if they dont intersect, return an empty array of points
        if (!this.boundary.intersects(rectArea)) {
            return;
        }

        for (let p of this.points) {
            // if the point is within the search box, add it to the list
            if (rectArea.contains(p)) {
                pointsFound.push(p);
            }
        }

        // if this region was split into subsection, check if intersects with those sections too
        if (alreadyDivided) {
            this.ne.query(rectArea, pointsFound);
            this.nw.query(rectArea, pointsFound);
            this.se.query(rectArea, pointsFound);
            this.sw.query(rectArea, pointsFound);
        }
    }
}

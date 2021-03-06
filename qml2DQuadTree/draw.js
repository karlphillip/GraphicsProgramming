// helper functions for drawing inside Canvas

function line(ctx, x1, y1, x2, y2, color, lineWidth=1) {
    ctx.save();
    ctx.beginPath();
    ctx.lineWidth = lineWidth;
    ctx.strokeStyle = color;
    ctx.moveTo(x1, y1);
    ctx.lineTo(x2, y2);
    ctx.closePath();
    ctx.stroke();
    ctx.restore();
}

function circle(ctx, x, y, radius, color) {
    ctx.save();
    ctx.beginPath()
    ctx.strokeStyle = color;
    ctx.fillStyle = color;
    ctx.arc(x, y, radius, 0, 360, false);
    ctx.fill();
    ctx.closePath()
    ctx.restore();
}

function rect(ctx, x, y, w, h, color, fill=true, lineWidth=1) {
    ctx.save();
    ctx.beginPath()

    // border width and color
    ctx.lineWidth = lineWidth;
    ctx.strokeStyle = color;
    ctx.rect(x, y, w, h);

    // fill color
    if (fill) {
        ctx.fillStyle = color;
        ctx.fill();
    }

    ctx.stroke();
    ctx.closePath()
    ctx.restore();
}

function rectCenter(ctx, x, y, w, h, color, borderColor, border=true) {
    ctx.save();
    ctx.beginPath();

    x -= w / 2;
    y -= h / 2;

    ctx.strokeStyle = borderColor;

    ctx.rect(x, y, w, h);
    ctx.fillStyle = color;
    ctx.fill();

    if (!border) {
        ctx.lineWidth = 1;
    }

    ctx.stroke();
    ctx.closePath()
    ctx.restore();
}

function rectCenter(ctx, x, y, w, h, color, borderColor, fill=true, lineWidth=1) {
    // move x,y to the left/top
    x -= w / 2;
    y -= h / 2;

    ctx.save();
    ctx.beginPath()

    // border width and color
    ctx.lineWidth = lineWidth;
    ctx.strokeStyle = borderColor;
    ctx.rect(x, y, w, h);

    // fill color
    if (fill) {
        ctx.fillStyle = color;
        ctx.fill();
    }

    ctx.stroke();
    ctx.closePath()
    ctx.restore();
}

function text(ctx, text, x, y, color, font="20px sans-serif") {
    ctx.save();
    ctx.font = font;
    ctx.fillStyle = color;
    ctx.fillText(qsTr(text), x, y);
    ctx.stroke();
    ctx.restore();
}

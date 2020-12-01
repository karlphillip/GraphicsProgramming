// helper functions for drawing inside Canvas

function line(ctx, x1, y1, x2, y2, color) {
    ctx.lineWidth = 1;
    ctx.strokeStyle = color;
    ctx.beginPath();
    ctx.moveTo(x1, y1);
    ctx.lineTo(x2, y2);
    ctx.closePath();
    ctx.stroke();
}

function circle(ctx, x, y, radius, color) {
    ctx.fillStyle = color;
    ctx.strokeStyle = "transparent";
    ctx.arc(x, y, radius, 0, 2*Math.PI, false);
    ctx.fill();
}

function rect(ctx, x, y, w, h, color) {
    ctx.fillStyle = color;
    ctx.fillRect(x, y, w, h);
    ctx.fill()
    ctx.stroke();
}

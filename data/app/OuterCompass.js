import { adjustOrigin } from './Utils.js';

export function createOuterCompass(cfg, path) {
  // Green, right arc
  const arcRight = new Konva.Arc({
    id: 'green_zone',
    x: cfg.center.x,
    y: cfg.center.y,
    innerRadius: cfg.radius.lvl2,
    outerRadius: cfg.radius.lvl1,
    angle: -90,
    clockwise: true,
    fillLinearGradientStartPoint: { x: -cfg.radius.lvl1 / 3, y: -cfg.radius.lvl1 / 3 },
    fillLinearGradientEndPoint: { x: cfg.radius.lvl1 / 3, y: cfg.radius.lvl1 / 3 },
    fillLinearGradientColorStops: [0, cfg.colors.green, 1, cfg.colors.darkGray],
    strokeWidth: 0.2,
    stroke: cfg.colors.darkGray,
  });

  // Red, left arc
  const arcLeft = new Konva.Arc({
    id: 'red_zone',
    x: cfg.center.x,
    y: cfg.center.y,
    innerRadius: cfg.radius.lvl2,
    outerRadius: cfg.radius.lvl1,
    angle: -180,
    clockwise: true,
    fillLinearGradientStartPoint: { x: cfg.radius.lvl1 / 3, y: -cfg.radius.lvl1 / 3 },
    fillLinearGradientEndPoint: { x: -cfg.radius.lvl1 / 3, y: cfg.radius.lvl1 / 3 },
    fillLinearGradientColorStops: [0, cfg.colors.red, 1, cfg.colors.darkGray],
    strokeWidth: 0.2,
    stroke: cfg.colors.darkGray,
  });

  // Bottom gray arc
  const arcBottom = new Konva.Arc({
    id: 'gray_zone',
    x: cfg.center.x,
    y: cfg.center.y,
    innerRadius: cfg.radius.lvl2,
    outerRadius: cfg.radius.lvl1,
    angle: 180,
    fill: cfg.colors.darkGray,
    strokeWidth: 0.2,
    stroke: cfg.colors.darkGray,
  });

  const north = new Konva.Line({
    id: 'north_line',
    x: cfg.center.x,
    y: cfg.center.y,
    points: [0, -cfg.radius.lvl2, 0, -cfg.radius.lvl1],
    stroke: cfg.colors.lightGray,
    strokeWidth: 1,
  });

  // Group all arcs to create full circle
  const outerCompass = new Konva.Group({
    id: 'outer_compass',
    name: 'outerCompass',
  });

  // Add all abobe to the group
  outerCompass.add(arcLeft, arcRight, arcBottom, north);

  // Create digits & dots
  for (let i = 0; i < cfg.NUM_LABELS; i++) {
    const pt = path.getPointAtLength((path.getLength() / cfg.NUM_LABELS) * i);

    // set label value
    const value = (360 / cfg.NUM_LABELS) * i;
    let num = `${value}`;

    // correct value to be in the range 180 to 0 degrees
    if (num > 180) {
      num = Math.abs((num % 180) - 180);
    }

    if (!(i % 3) && i !== 0 && i !== 18) {
      const txt = new Konva.Text({
        x: pt.x,
        y: pt.y,
        text: num,
        fontSize: cfg.rings.r12.width / 2,
        fontStyle: 'bold',
        fill: cfg.colors.lightGray,
        rotation: value,
      });

      // Adjust the offset of the labeles
      txt.offsetX(txt.getTextWidth() / 2);
      txt.offsetY((cfg.rings.r12.width - txt.fontSize()) / 2.5);

      outerCompass.add(txt);
    } else if (i !== 0) {
      const c = new Konva.Circle({
        x: pt.x,
        y: pt.y,
        radius: cfg.rings.r12.width / 12,
        fill: cfg.colors.lightGray,
      });

      outerCompass.add(c);
    }
  }

  // outerCompass.add(path);

  // Make the origin to the center of the circles
  adjustOrigin(outerCompass, cfg.center);

  return outerCompass;
}

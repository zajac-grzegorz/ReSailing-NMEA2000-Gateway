import { adjustOrigin } from './Utils.js';

export function createInnerCompass(cfg, path) {
  // Inner ring - compass
  const innerCompassRing = new Konva.Ring({
    id: 'inner_compass_ring',
    x: cfg.center.x,
    y: cfg.center.y,
    innerRadius: cfg.radius.lvl4,
    outerRadius: cfg.radius.lvl3,
    fill: cfg.colors.lightGray,
  });

  // Group all inner compass
  const innerCompass = new Konva.Group({
    id: 'inner_compass',
    name: 'innerCompass',
  });

  // Add all arcs to the group
  innerCompass.add(innerCompassRing);

  // Create digits & dots
  for (let i = 0; i < cfg.NUM_LABELS; i++) {
    const pt = path.getPointAtLength((path.getLength() / cfg.NUM_LABELS) * i);

    // set label value
    const value = (360 / cfg.NUM_LABELS) * i;
    let num = `${value}`;

    if (num == 0) {
      num = 'N';
    }

    if (!(i % 3)) {
      const txt = new Konva.Text({
        x: pt.x,
        y: pt.y,
        text: num,
        fontSize: cfg.rings.r12.width / 2,
        fontStyle: 'bold',
        fill: cfg.colors.darkGray,
        rotation: value,
      });

      // Adjust the offset of the labeles
      txt.offsetX(txt.getTextWidth() / 2);
      txt.offsetY((cfg.rings.r12.width - txt.fontSize()) / 2.5);

      innerCompass.add(txt);
    } else if (i !== 0) {
      const c = new Konva.Circle({
        x: pt.x,
        y: pt.y,
        radius: cfg.rings.r12.width / 12,
        fill: cfg.colors.darkGray,
      });

      innerCompass.add(c);
    }
  }

  // innerCompass.add(path);

  // Initial heading
  innerCompass.hdg = 0;

  // Make the origin to the center of the circles
  adjustOrigin(innerCompass, cfg.center);

  return innerCompass;
}

export function createInnerCompassBox(cfg, path) {
  const pt = path.getPointAtLength(0);

  const box = new Konva.Rect({
    x: pt.x,
    y: pt.y,
    width: cfg.rings.r34.width * 2.2,
    height: cfg.rings.r34.width * 1.3,
    fill: cfg.colors.lightGray,
    cornerRadius: 2,
    opacity: 1,
    stroke: 'black',
    strokeWidth: 0.3,
  });

  box.offsetX(box.width() / 2);
  box.offsetY((box.height() / 2) * 0.9);

  // Group all inner compass
  const compassBox = new Konva.Group({
    id: 'inner_compass_box',
    name: 'innerCompassBox',
  });

  const txt = new Konva.Text({
    id: 'compass_box_text',
    x: pt.x,
    y: pt.y,
    text: '000',
    fontSize: box.height() * 0.8,
    fontStyle: 'Bold',
    fill: cfg.colors.darkGray,
  });

  txt.offsetX(txt.width() / 2);
  txt.offsetY(txt.height() * 0.4);

  compassBox.add(box, txt);

  return compassBox;
}

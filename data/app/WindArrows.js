import { adjustOrigin } from './Utils.js';

export function createTrueWindArrow(cfg, path) {
  const trueWindArrow = new Konva.Group({
    id: 'true_wind_arrow',
    name: 'trueWindArrow',
    ext: 'twa',
  });

  const pt = path.getPointAtLength(0);

  const twaTri = new Konva.RegularPolygon({
    id: 'twa_arrow',
    x: pt.x,
    y: pt.y,
    offsetY: cfg.rings.r12.width / 6,
    sides: 3,
    radius: -cfg.rings.r12.width / 1.1,
    fill: cfg.colors.orange,
    opacity: 0.95,
  });

  const twaTxt = new Konva.Text({
    id: 'twa_text',
    x: pt.x,
    y: pt.y,
    text: '10.5',
    offsetY: twaTri.getSelfRect().height,
    fontSize: twaTri.getSelfRect().height * 0.5,
    fontStyle: 'Bold',
    fill: cfg.colors.orange,
  });

  twaTxt.offsetX(twaTxt.getTextWidth() / 2);

  const twaTriTxt = new Konva.Text({
    id: 'twa_arrow_text',
    x: pt.x,
    y: pt.y,
    text: 'T',
    offsetY: twaTri.getSelfRect().height * 0.3,
    fontSize: twaTri.getSelfRect().height * 0.6,
    fontStyle: 'Bold',
    fill: cfg.colors.lightGray,
  });

  twaTriTxt.offsetX(twaTriTxt.getTextWidth() / 2);

  trueWindArrow.add(twaTri, twaTriTxt, twaTxt);
  adjustOrigin(trueWindArrow, cfg.center);

  // Initial angle value
  trueWindArrow.angle = 0;

  return trueWindArrow;
}

export function createAparentWindArrow(cfg, path) {
  const aparentWindArrow = new Konva.Group({
    id: 'aparent_wind_arrow',
    name: 'aparentWindArrow',
    ext: 'awa',
  });

  const pt = path.getPointAtLength(0);

  const awaTri = new Konva.RegularPolygon({
    id: 'awa_arrow',
    x: pt.x,
    y: pt.y,
    offsetY: -cfg.rings.r12.width / 7,
    sides: 3,
    radius: -cfg.rings.r12.width / 1.1,
    fill: cfg.colors.darkBlue,
    fill: cfg.colors.lightBlue,
    scaleY: 2,
    opacity: 0.95,
  });

  const awaTxt = new Konva.Text({
    id: 'awa_text',
    x: pt.x,
    y: pt.y,
    text: '10.5',
    offsetY: awaTri.getSelfRect().height,
    fontSize: awaTri.getSelfRect().height * 0.5,
    fontStyle: 'Bold',
    fill: cfg.colors.lightBlue,
  });

  awaTxt.offsetX(awaTxt.getTextWidth() / 2);

  const awaTriTxt = new Konva.Text({
    id: 'awa_arrow_text',
    x: pt.x,
    y: pt.y,
    text: 'A',
    offsetY: awaTri.getSelfRect().height * 0.3,
    fontSize: awaTri.getSelfRect().height * 0.6,
    fontStyle: 'Bold',
    fill: cfg.colors.lightGray,
  });

  awaTriTxt.offsetX(awaTriTxt.getTextWidth() / 2);

  aparentWindArrow.add(awaTri, awaTriTxt, awaTxt);
  adjustOrigin(aparentWindArrow, cfg.center);

  // Initial angle value
  aparentWindArrow.angle = 0;

  return aparentWindArrow;
}

const PADDING = 2;

export function createSmallDisplay(cfg, param) {
  const dispGroup = new Konva.Group({
    id: `${param.name}_display`,
    name: `${param.name}Display`,
  });

  // const rect = new Konva.Rect({
  //   width: 45,
  //   height: 24,
  //   fill: 'black',
  //   stroke: cfg.colors.lighterGray,
  //   strokeWidth: 0.5,
  //   cornerRadius: 1,
  // });
  const rect = new Konva.Rect({
    width: 45,
    height: 24,
    fill: 'black',
    // stroke: cfg.colors.lighterGray,
    // strokeWidth: 0.5,
    cornerRadius: 1,
  });

  const line = new Konva.Line({
    points: [0, 23, 0, 25, 45, 25, 45, 23],
    stroke: cfg.colors.lighterGray,
    strokeWidth: 0.5,
    // lineCap: 'round',
    // lineJoin: 'round',
  });

  const paramName = new Konva.Text({
    x: PADDING,
    y: PADDING,
    text: param.name,
    fontSize: 6,
    fontFamily: 'Calibri',
    fill: cfg.colors.grayText,
  });

  const paramUnit = new Konva.Text({
    y: PADDING,
    text: param.unit,
    fontSize: 6,
    fontFamily: 'Calibri',
    fill: cfg.colors.grayText,
  });

  paramUnit.x(rect.width() - paramUnit.getTextWidth() - PADDING);

  const paramValue = new Konva.Text({
    text: param.value,
    fontSize: 18,
    fontStyle: 'bold',
    fontFamily: 'Calibri',
    fill: 'white',
  });

  //   paramValue.text(paramValue.text().padStart(3, '0'));
  paramValue.x((rect.width() - paramValue.getTextWidth()) / 2);
  paramValue.y(rect.height() - paramValue.height() + 2);

  dispGroup.add(rect, line, paramName, paramUnit, paramValue);

  return dispGroup;
}

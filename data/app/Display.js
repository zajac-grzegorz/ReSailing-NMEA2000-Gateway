const PADDING = 2;

export function createSmallDisplay(cfg, param) {
  const dispGroup = new Konva.Group({
    id: `${param.name}_display`,
    name: `${param.name}Display`,
    draggable: true
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
    width: cfg.outer / 2,
    height: cfg.outer / 4,
    fill: 'black',
    // stroke: 'white',
    // strokeWidth: 0.5,
    // cornerRadius: 5,
  });

  const paramName = new Konva.Text({
    x: PADDING,
    y: PADDING,
    text: param.name,
    fontSize: rect.getSelfRect().height / 5,
    fontFamily: 'Calibri',
    fill: cfg.colors.grayText,
  });

  const line = new Konva.Line({
    points: [paramName.getSelfRect().x, paramName.getSelfRect().height / 1.2 , paramName.getSelfRect().x, paramName.getSelfRect().height * 1.1, paramName.getSelfRect().width*1.1 + 1, paramName.getSelfRect().height * 1.1, paramName.getSelfRect().width*1.1 +1, paramName.getSelfRect().height / 1.2],
    stroke: "white",
    strokeWidth: 0.5,
    // lineCap: 'round',
    // lineJoin: 'round',
  });

  const paramUnit = new Konva.Text({
    y: PADDING,
    text: param.unit,
    fontSize: rect.getSelfRect().height / 4,
    fontFamily: 'Calibri',
    fill: cfg.colors.grayText,
  });

  paramUnit.x(rect.width() - paramUnit.getTextWidth() - PADDING);

  const paramValue = new Konva.Text({
    text: param.value,
    fontSize: rect.getSelfRect().height / 1.2,
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

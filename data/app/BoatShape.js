export function createBoatShape(cfg) {
  const boatShape = new Konva.Path({
    id: 'boat_shape',
    x: cfg.center.x,
    y: cfg.center.y,
    data: `
    M ${-cfg.radius.lvl4 / 2.5} ${cfg.radius.lvl4 / 1.5}
    Q ${-cfg.radius.lvl4 / 2.5} ${-cfg.radius.lvl4 / 3} 0 ${-cfg.radius.lvl4 / 1.1}
    Q ${cfg.radius.lvl4 / 2.5} ${-cfg.radius.lvl4 / 3} ${cfg.radius.lvl4 / 2.5} ${cfg.radius.lvl4 / 1.5}
  `,
    stroke: cfg.colors.lightGray,
    strokeWidth: 1,
    strokeLinearGradientStartPoint: { x: cfg.center.x, y: -cfg.radius.lvl1 },
    strokeLinearGradientEndPoint: { x: cfg.center.x, y: cfg.radius.lvl1 },
    strokeLinearGradientColorStops: [0, cfg.colors.lightGray, 1, cfg.colors.darkGray],
  });

  return boatShape;
}

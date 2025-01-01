export const MAX_ANGLE = 360;
export const MAX_ANGLE_HALF = 180;

export const direction = {
  CW: 1,
  CCW: -1,
};

export function adjustOrigin(shape, center) {
  shape.offsetX(center.x);
  shape.offsetY(center.y);
  shape.x(center.x);
  shape.y(center.y);
}

export function createOuterCompassPath(cfg) {
  // Create path in the center of the outer compass
  const outerRadius = cfg.radius.lvl1 - cfg.rings.r12.width / 2;

  const outerCompassPath = new Konva.Path({
    id: 'outer_compass_path',
    x: 0,
    y: 0,
    data: `
    M ${cfg.center.x} ${cfg.center.y - outerRadius} 
    a ${outerRadius} ${outerRadius} 0 0 1 0 ${outerRadius * 2 - 0.0001} 
    a ${outerRadius} ${outerRadius} 0 0 1 0 ${-outerRadius * 2}
    `,
    stroke: 'white',
    strokeWidth: 0.5,
  });

  return outerCompassPath;
}

export function createInnerCompassPath(cfg) {
  // Create path in the center of the inner compass
  const innerRadius = cfg.radius.lvl3 - cfg.rings.r34.width / 2;

  const innerCompassPath = new Konva.Path({
    id: 'inner_compass_path',
    x: 0,
    y: 0,
    data: `
    M ${cfg.center.x} ${cfg.center.y - innerRadius} 
    a ${innerRadius} ${innerRadius} 0 0 1 0 ${innerRadius * 2 - 0.0001} 
    a ${innerRadius} ${innerRadius} 0 0 1 0 ${-innerRadius * 2}
    `,
    stroke: 'black',
    strokeWidth: 0.5,
  });

  return innerCompassPath;
}

export function getCalculatedAngle(node, start, end, direction) {
  let rot = node.rotation();

  // log.debug({
  //   elem: node.id(),
  //   rot: rot,
  //   old: start,
  //   new: end,
  //   dir: direction,
  // });

  let distance1 = end - start;
  let distance2 = 360 - distance1;
  let distance = Math.min(distance1, distance2);

  log.debug(`Dist1: ${distance}`);

  if (distance < -MAX_ANGLE_HALF) {
    distance += MAX_ANGLE;
  }
  log.debug(`Dist2: ${distance}`);

  if (start < end && distance1 > MAX_ANGLE_HALF) {
    distance = -distance;
  }

  log.debug(`Distance: ${distance}`);

  // Clock Wise = 1, Counter Clock Wise = -1
  rot = rot + distance * direction;
  log.debug(`Rot: ${rot}`);

  return rot;
}

export function normalizeRotationAngle(node) {
  let rot = node.rotation();

  // Normalize to range 0-360
  if (rot < 0) {
    rot += MAX_ANGLE;
  } else if (rot >= MAX_ANGLE) {
    rot -= MAX_ANGLE;
  }

  return rot;
}

// export function getCalculatedCompassRotation(node, start, end) {
//   let rot = node.rotation();

//   log.info({
//     elem: node.id(),
//     rot: rot,
//     old: start,
//     new: end,
//   });

//   let distance1 = end - start;
//   let distance2 = 360 - distance1;
//   let distance = Math.min(distance1, distance2);

//   log.debug(`Dist1: ${distance}`);

//   if (distance < -MAX_ANGLE_HALF) {
//     distance += MAX_ANGLE;
//   }
//   log.debug(`Dist2: ${distance}`);

//   if (start < end && distance1 > MAX_ANGLE_HALF) {
//     distance = -distance;
//   }

//   log.debug(`Distance: ${distance}`);

//   let rot1 = rot - distance;
//   log.debug(`Rot: ${rot1}`);

//   return rot1;
// }

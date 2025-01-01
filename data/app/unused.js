// Calculate new angle for the rotation with the shortest path
function getCalculatedAngle(node, startAngle, endAngle) {
  const rot = node.rotation();
  // console.log(`current rotation before: ${rot}`);

  // console.log({
  //   elem: node.id(),
  //   rot: rot,
  //   old: startAngle,
  //   new: endAngle,
  // });

  if (rot > 360) {
    node.rotation(rot - 360);
  } else if (rot < -360) {
    node.rotation(rot + 360);
  }

  let ang = calculateShortestWay(startAngle, endAngle);

  const diff = node.rotation() + ang;
  // console.log(`Angle to rotate 1: ${-ang}`);

  if (diff > 180) {
    ang -= 360;
  }

  // console.log(`Angle to rotate 2: ${-ang}`);
  // console.log(`current rotation before: ${node.rotation()}`);

  return ang;
}

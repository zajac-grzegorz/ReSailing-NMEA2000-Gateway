// Sample data - use fetch to get fresh data
export const data = {
  twa: 0,
  tws: 0,
  awa: 0,
  aws: 0,
  hdg: 0,
};

function updateData() {
  data.twa = getRandom(0, 359);
  data.tws = getRandom(0, 400) / 10.0;
  data.awa = getRandom(0, 359);
  data.aws = getRandom(0, 450) / 10.0;
  data.hdg = getRandom(0, 359);

  log.trace('--> new data');
}

function getRandom(min, max) {
  min = Math.ceil(min);
  max = Math.floor(max);
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

// Update data periodically
export function testAutomatic(node) {
  const intvl = setInterval(() => {
    updateData();
  }, 2000);

  node.off('click');

  // add click listener
  node.on('click', function () {
    log.trace('Stopped');
    clearInterval(intvl);
  });
}

// Update data when clicked on the shape
export function testManual(node, cb) {
  node.off('click');

  // add click listener
  node.on('click', function () {
    setTimeout(() => {
      updateData();
      cb(data);
    }, 1000);
  });
}

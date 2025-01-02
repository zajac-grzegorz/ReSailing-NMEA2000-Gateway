import { Config } from './Config.js';
import { createOuterCompass } from './OuterCompass.js';
import { createInnerCompass, createInnerCompassBox } from './InnerCompass.js';
import { createBoatShape } from './BoatShape.js';
import { createTrueWindArrow, createAparentWindArrow } from './WindArrows.js';
import { createSmallDisplay } from './Display.js';
import * as Utils from './Utils.js';
import { testAutomatic, testManual, data } from './TestUtils.js';

// Set log level
log.setLevel('info');

// Repaint the canvas after resize
window.onresize = () => location.reload();

// Konva.pixelRatio = 1;

// Stage for the canvas
const stage = new Konva.Stage({
  container: 'container',
  width: window.innerWidth,
  height: window.innerHeight,
});

// Main layer
const layer = new Konva.Layer();
stage.add(layer);

// Animation layer
const animLayer = new Konva.Layer();
stage.add(animLayer);

// Instantiate new configuration object
const cfg = new Config(stage.width(), stage.height());

// Instantiate outer and inner compass path
const outerPath = Utils.createOuterCompassPath(cfg);
const innerPath = Utils.createInnerCompassPath(cfg);

// Instantiate compass elements
const outerCompass = createOuterCompass(cfg, outerPath);
const innerCompass = createInnerCompass(cfg, innerPath);
const compassBox = createInnerCompassBox(cfg, innerPath);
const boatShape = createBoatShape(cfg);

// Add arrows to the layer
const twaArrow = createTrueWindArrow(cfg, outerPath);
const awaArrow = createAparentWindArrow(cfg, outerPath);

layer.add(outerCompass, boatShape);
animLayer.add(innerCompass, compassBox, awaArrow, twaArrow);

// TWA always on top
twaArrow.moveToTop();

// // Middle point in RED
// const wptBrg = new Konva.Circle({
//   id: 'wpt_bearing',
//   x: cfg.center.x,
//   y: cfg.center.y,
//   radius: 1,
//   fill: cfg.colors.red,
// });

// const simpleText = new Konva.Text({
//   x: cfg.center.x,
//   y: cfg.center.y,
//   text: '0',
//   fontSize: 20,
//   fontFamily: 'Calibri',
//   fill: 'red',
// });

// layer.add(wptBrg, simpleText);

// Update wind arrow value when both arrows overlap
const awaText = stage.findOne(`#awa_text`);

function updateWindArrowsText() {
  const awa = Utils.normalizeRotationAngle(awaArrow);
  const twa = Utils.normalizeRotationAngle(twaArrow);

  // Compare normalized wind angles
  if (Math.abs(twa - awa) < 10) {
    awaText.opacity(0.2);

    log.debug(`twa: ${twa}, awa: ${awa}`);
  } else if (Utils.MAX_ANGLE - Math.abs(twa - awa) < 10) {
    awaText.opacity(0.2);

    log.debug(`twa: ${twa}, awa: ${awa}`);
  } else {
    awaText.opacity(1);
  }
}

// Default interval for animation duration when update wind & compass components
let updateInterval = 0.5;

// Update position of the selected wind arrow
function updateWind(type, data) {
  const arrow = stage.findOne(`#${type}`);
  const ext = arrow.getAttr('ext');
  const txt = stage.findOne(`#${ext}_text`);

  let speed = 0;
  let angle = 0;

  if (ext === 'awa') {
    speed = data.aws;
    angle = data.awa;
  } else if (ext === 'twa') {
    speed = data.tws;
    angle = data.twa;
  }

  txt.text(speed.toFixed(1));
  txt.offsetX(txt.getTextWidth() / 2);

  const ang = Utils.getCalculatedAngle(arrow, arrow.angle, angle, Utils.direction.CW);
  arrow.angle = angle;

  arrow.to({
    duration: updateInterval,
    rotation: ang,
    onUpdate: () => {
      if (ext === 'awa') {
        updateWindArrowsText();
      }
    },
    onFinish: () => {
      const oldRot = arrow.rotation();
      log.debug(`current rotation after 1: ${oldRot}`);

      const rot = Utils.normalizeRotationAngle(arrow);

      // Update final rotation
      arrow.rotation(rot);

      if (rot !== angle) {
        log.error(`Mismatch rot = ${rot} and angle = ${angle}`);
      }

      log.debug(`final rotation: ${rot}`);
    },
  });
}

let hdgNeedUpdate = true;

// Update rotation of the inner compas to match a heading
function updateHeading(data) {
  const txt = stage.findOne(`#compass_box_text`);
  const compass = stage.findOne(`#inner_compass`);
  const heading = data.hdg;

  const ang = Utils.getCalculatedAngle(compass, compass.hdg, heading, Utils.direction.CCW);
  compass.hdg = heading;

  // Update text at the top of the compass
  txt.text(`${heading}`.padStart(3, '0'));
  txt.offsetX(txt.width() / 2);

  compass.to({
    node: compass,
    duration: updateInterval,
    rotation: ang,
    onFinish: () => {
      let rot = compass.rotation();
      log.debug(`current rotation after 1: ${rot}`);

      if (rot < 0) {
        compass.rotation(rot + Utils.MAX_ANGLE);
      } else if (rot > Utils.MAX_ANGLE) {
        compass.rotation(rot - Utils.MAX_ANGLE);
      }

      // Get updated final rotation
      rot = compass.rotation();

      if (rot !== 0 && heading != 0 && Utils.MAX_ANGLE - rot !== heading) {
        log.error('Heading mismatch');
      }
      
      hdgNeedUpdate = true;
      // log.debug(`final rotation: ${rot}`);
    },
  });
}

// Update all moving/rotating parts of the instrument
function updateInstrument(data) {
  updateWind('true_wind_arrow', data);
  updateWind('aparent_wind_arrow', data);

  if (hdgNeedUpdate)
  {
    hdgNeedUpdate = false;
    updateHeading(data);
  }
}

if (!!window.EventSource) {
  let source = new EventSource('/re');

  source.addEventListener('open', function (e) {
    log.info("Events Connected");
  }, false);
  
  source.addEventListener('error', function (e) {
    if (e.target.readyState != EventSource.OPEN) {
      log.error("Events Disconnected");
    }
  }, false);
  
  source.addEventListener('update_interval', function (e) {
    log.debug("Interval", e.data);

    // Period comes from ESP in miliseconds, therefore /1000.
    // additionally animation interval needs to be much smaller than the period from ESP
    // so /2 is here. Animation must complete before new update is requested. 
    let interval = parseInt(e.data);
    
    updateInterval = Math.min(updateInterval, interval / 1000 / 2);
    log.info("updateInterval", updateInterval);

  }, false);
  source.addEventListener('update_data', function (e) {
    // console.log("update", e.data);

    const objData = JSON.parse(e.data);
    data.awa = objData.AWA;
    data.aws = objData.AWS;
    data.twa = objData.TWA;
    data.tws = objData.TWS;
    data.hdg = objData.Heading;
    
    updateInstrument(data);
  }, false);
}

////////////////////

const cogParam = {
  name: 'COG',
  value: 10,
  unit: '°M',
};

const twdParam = {
  name: 'TWD',
  value: 165,
  unit: '°M',
};

const sogParam = {
  name: 'SOG',
  value: 12.5,
  unit: 'kn',
};

const cogDisp = createSmallDisplay(cfg, cogParam);
const sogDisp = createSmallDisplay(cfg, sogParam);
const twdDisp = createSmallDisplay(cfg, twdParam);

layer.add(cogDisp, sogDisp, twdDisp);
cogDisp.x(5);
cogDisp.y(5);
// cogDisp.scale({ x: 0.5, y: 0.5 });

sogDisp.x(5);
sogDisp.y(35);

twdDisp.x(5);
twdDisp.y(65);

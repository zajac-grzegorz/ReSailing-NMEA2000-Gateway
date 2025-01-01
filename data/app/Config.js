class Config {
  constructor(w, h) {
    this.width = w;
    this.height = h;
    this.outer = Math.min(this.width, this.height) / this.LVL1_RADIUS_FACTOR;

    // center property
    this.center = {};
    this.center.x = this.width / 2;
    this.center.y = this.height / 2;

    // radius property
    this.radius = {};
    this.radius.lvl1 = this.outer;
    this.radius.lvl2 = this.outer * this.LVL2_RADIUS_FACTOR;
    this.radius.lvl3 = this.outer * this.LVL3_RADIUS_FACTOR;
    this.radius.lvl4 = this.outer * this.LVL4_RADIUS_FACTOR;

    // rings property
    this.rings = {};
    this.rings.r12 = {};
    this.rings.r12.width = this.radius.lvl1 * (1 - this.LVL2_RADIUS_FACTOR);
    this.rings.r34 = {};
    this.rings.r34.width = this.radius.lvl3 - this.radius.lvl4;
  }

  NUM_LABELS = 36;
  LVL1_RADIUS_FACTOR = 2.4;
  LVL2_RADIUS_FACTOR = 0.9;
  LVL3_RADIUS_FACTOR = 0.86;
  LVL4_RADIUS_FACTOR = 0.76;

  colors = {
    lightGray: 'rgb(230, 230, 230)',
    grayText: 'rgb(180, 180, 180)',
    lighterGray: 'rgb(50, 50, 50)',
    darkGray: 'rgb(20, 20, 20)',
    green: 'rgb(20, 140, 0)',
    red: 'rgb(180, 0, 0)',
    lightBlue: 'rgb(45, 150, 255)',
    darkBlue: 'rgb(0, 70, 200)',
    orange: 'rgb(250, 150, 0)',
  };
}

export { Config };

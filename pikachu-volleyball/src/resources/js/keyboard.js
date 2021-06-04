/**
 * This module takes charge of the user input via keyboard
 */
'use strict';
import { PikaUserInput } from './physics.js';

// ====================== Websocket receive ======================
var data1 = { right: 0, left: 0, up: 0, down: 0, hit: 0, jump: 0 };
var data2 = { right: 0, left: 0, up: 0, down: 0, hit: 0, jump: 0 };
function connect1() {
  const ws1 = new WebSocket("ws://127.0.0.1:8866");
  ws1.onopen = function () {
    // Web Socket is connected, send data using send()
    console.log("onopen 1")
    ws1.send("Message to send");
  };
  ws1.onmessage = function (evt) {
    data1 = JSON.parse(evt.data);
  };
  ws1.onclose = function () {
    console.log("disconnect of p1");
    data1 = { right: 0, left: 0, up: 0, down: 0, hit: 0, jump: 0 };
    setTimeout(function () {
      connect1();
    }, 1500);
  }
}

function connect2() {
  const ws2 = new WebSocket("ws://127.0.0.1:8867");
  ws2.onopen = function () {
    // Web Socket is connected, send data using send()
    console.log("onopen 2")
    ws2.send("Message to send");
  };
  ws2.onmessage = function (evt) {
    data2 = JSON.parse(evt.data);
  };
  ws2.onclose = function () {
    console.log("disconnect of p2");
    data2 = { right: 0, left: 0, up: 0, down: 0, hit: 0, jump: 0 };
    setTimeout(function () {
      connect2();
    }, 1500);
  }
}


connect1();
connect2();

// ====================== Websocket receive ======================
/**
 * Class representing a keyboard used to contorl a player
 */
export class PikaKeyboard extends PikaUserInput {
  /**
   * Create a keyboard used for game controller
   * left, right, up, down, powerHit: KeyboardEvent.code value for each
   * Refer {@link https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/code/code_values}
   * @param {string} left KeyboardEvent.code value of the key to use for left
   * @param {string} right KeyboardEvent.code value of the key to use for right
   * @param {string} up KeyboardEvent.code value of the key to use for up
   * @param {string} down KeyboardEvent.code value of the key to use for down
   * @param {string} powerHit KeyboardEvent.code value of the key to use for power hit or selection
   * @param {string} downRight KeyboardEvent.code value of the key to use for having the same effect
   *                           when pressing down key and right key at the same time (Only player 1
   *                           has this key)
   */
  constructor(left, right, up, down, powerHit, downRight = null) {
    super();

    /** @type {boolean} */
    this.powerHitKeyIsDownPrevious = false;

    /** @type {Key} */
    this.leftKey = new Key(left);
    /** @type {Key} */
    this.rightKey = new Key(right);
    /** @type {Key} */
    this.upKey = new Key(up);
    /** @type {Key} */
    this.downKey = new Key(down);
    /** @type {Key} */
    this.powerHitKey = new Key(powerHit);
    /** @type {Key} */
    this.downRightKey = new Key(downRight);
  }

  /**
   * Get xDirection, yDirection, powerHit input from the keyboard.
   * This method is for freezing the keyboard input during the process of one game frame.
   */
  /* 
  ===================== Original Keyboard Input ====================
  getInput() {
    if (this.leftKey.isDown) {
      this.xDirection = -1;
    } else if (
      this.rightKey.isDown ||
      (this.downRightKey && this.downRightKey.isDown)
    ) {
      this.xDirection = 1;
    } else {
      this.xDirection = 0;
    }

    if (this.upKey.isDown) {
      this.yDirection = -1;
    } else if (
      this.downKey.isDown ||
      (this.downRightKey && this.downRightKey.isDown)
    ) {
      this.yDirection = 1;
    } else {
      this.yDirection = 0;
    }

    const isDown = this.powerHitKey.isDown;
    if (!this.powerHitKeyIsDownPrevious && isDown) {
      this.powerHit = 1;
    } else {
      this.powerHit = 0;
    }
    this.powerHitKeyIsDownPrevious = isDown;
  }
  ===================== Original Keyboard Input ====================
  */
  // ===================== Websocket SMT32 Input ====================

  //Player 1
  getInput() {
    if (data1.left) {
      this.xDirection = -1;
    }
    else if (data1.right) {
      this.xDirection = 1;
    }
    else {
      this.xDirection = 0;
    }

    if (data1.up) {
      this.yDirection = -1;
    }
    else if (data1.down) {
      this.yDirection = 1;
    }
    else {
      this.yDirection = 0;
    }
    if (data1.jump) {
      this.jjump = 1;
    }
    else { this.jjump = 0; }

    if (data1.hit) {
      this.powerHit = 1;
    }
    else {
      this.powerHit = 0;
    }

    //===================== Original Keyboard Input ====================
    /*if (this.leftKey.isDown) {
      this.xDirection = -1;
    } else if (
      this.rightKey.isDown ||
      (this.downRightKey && this.downRightKey.isDown)
    ) {
      this.xDirection = 1;
    } else {
      this.xDirection = 0;
    }

    if (this.upKey.isDown) {
      this.yDirection = -1;
    } else if (
      this.downKey.isDown ||
      (this.downRightKey && this.downRightKey.isDown)
    ) {
      this.yDirection = 1;
    } else {
      this.yDirection = 0;
    }*/

    /*const isDown = this.powerHitKey.isDown;
    if (!this.powerHitKeyIsDownPrevious && isDown) {
      this.powerHit = 1;
    } else {
      this.powerHit = 0;
    }
    this.powerHitKeyIsDownPrevious = isDown;*/
    //===================== Original Keyboard Input ====================
  }

  //Player 2
  getInput2() {
    if (data2.left) {
      this.xDirection = -1;
    }
    else if (data2.right) {
      this.xDirection = 1;
    }
    else {
      this.xDirection = 0;
    }

    if (data2.up) {
      this.yDirection = -1;
    }
    else if (data2.down) {
      this.yDirection = 1;
    }
    else {
      this.yDirection = 0;
    }
    if (data2.jump) {
      this.jjump = 1;
    }
    else { this.jjump = 0; }

    if (data2.hit) {
      this.powerHit = 1;
    }
    else {
      this.powerHit = 0;
    }
  }

  // ===================== Websocket SMT32 Input ====================
  /**
   * Subscribe keydown, keyup event listners for the keys of this keyboard
   */
  subscribe() {
    this.leftKey.subscribe();
    this.rightKey.subscribe();
    this.upKey.subscribe();
    this.downKey.subscribe();
    this.powerHitKey.subscribe();
    this.downRightKey.subscribe();
  }

  /**
   * Unsubscribe keydown, keyup event listners for the keys of this keyboard
   */
  unsubscribe() {
    this.leftKey.unsubscribe();
    this.rightKey.unsubscribe();
    this.upKey.unsubscribe();
    this.downKey.unsubscribe();
    this.powerHitKey.unsubscribe();
    this.downRightKey.unsubscribe();
  }
}

/**
 * Class respresenting a key on a keyboard
 * refered: https://github.com/kittykatattack/learningPixi
 */
class Key {
  /**
   * Create a key
   * Refer {@link https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/code/code_values}
   * @param {string} value KeyboardEvent.code value of this key
   */
  constructor(value) {
    this.value = value;
    this.isDown = false;
    this.isUp = true;

    this.downListener = this.downHandler.bind(this);
    this.upListner = this.upHandler.bind(this);
    this.subscribe();
  }

  /**
   * When key downed
   * @param {KeyboardEvent} event
   */
  downHandler(event) {
    if (event.code === this.value) {
      this.isDown = true;
      this.isUp = false;
      event.preventDefault();
    }
  }

  /**
   * When key upped
   * @param {KeyboardEvent} event
   */
  upHandler(event) {
    if (event.code === this.value) {
      this.isDown = false;
      this.isUp = true;
      event.preventDefault();
    }
  }

  /**
   * Subscribe event listeners
   */
  subscribe() {
    window.addEventListener('keydown', this.downListener);
    window.addEventListener('keyup', this.upListner);
  }

  /**
   * Unsubscribe event listeners
   */
  unsubscribe() {
    window.removeEventListener('keydown', this.downListener);
    window.removeEventListener('keyup', this.upListner);
    this.isDown = false;
    this.isUp = true;
  }
}

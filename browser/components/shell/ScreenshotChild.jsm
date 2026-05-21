"use strict";

const EXPORTED_SYMBOLS = ["ScreenshotChild"];

class ScreenshotChild extends JSWindowActorChild {
  receiveMessage(message) {
    if (message.name == "TakeScreenshot") {
      return this.takeScreenshot(message.data);
    }
    return null;
  }

  async waitForReadyToPaint() {
    let { contentWindow, document } = this;

    let waitForTimeout = delay =>
      new Promise(resolve => contentWindow.setTimeout(resolve, delay));
    let boundedWait = (promise, delay) =>
      Promise.race([promise.catch(() => {}), waitForTimeout(delay)]);

    if (this.document.readyState != "complete") {
      await boundedWait(
        new Promise(resolve =>
          contentWindow.addEventListener("load", resolve, { once: true })
        ),
        10000
      );
    }

    if (document.fonts) {
      await boundedWait(document.fonts.ready, 5000);
    }

    await waitForTimeout(500);

    await boundedWait(
      new Promise(resolve =>
        contentWindow.requestAnimationFrame(() =>
          contentWindow.requestAnimationFrame(resolve)
        )
      ),
      1000
    );
  }

  async takeScreenshot(params) {
    await this.waitForReadyToPaint();

    let { fullWidth, fullHeight } = params;
    let { contentWindow } = this;

    let canvas = contentWindow.document.createElementNS(
      "http://www.w3.org/1999/xhtml",
      "html:canvas"
    );
    let context = canvas.getContext("2d");
    let width = contentWindow.innerWidth;
    let height = contentWindow.innerHeight;
    if (fullWidth) {
      width += contentWindow.scrollMaxX - contentWindow.scrollMinX;
    }
    if (fullHeight) {
      height += contentWindow.scrollMaxY - contentWindow.scrollMinY;
    }

    canvas.width = width;
    canvas.height = height;
    context.drawWindow(
      contentWindow,
      0,
      0,
      width,
      height,
      "rgb(255, 255, 255)"
    );

    return new Promise(resolve => canvas.toBlob(resolve));
  }
}

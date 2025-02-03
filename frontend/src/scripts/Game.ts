import * as TH from "three";
import { OrbitControls } from "three/examples/jsm/Addons.js";

export default class Game {
  private renderer: TH.WebGLRenderer;
  private scene: TH.Scene;
  private camera: TH.PerspectiveCamera;

  constructor(private canvas: HTMLCanvasElement) {
    this.renderer = new TH.WebGLRenderer({
      canvas: this.canvas,
      antialias: true,
    });
    this.renderer.setSize(canvas.clientWidth, canvas.clientHeight);

    this.scene = this.createScene();

    this.camera = new TH.PerspectiveCamera(
      75,
      canvas.clientWidth / canvas.clientHeight,
      0.1,
      30,
    );
    this.camera.position.z = 5;
    this.scene.add(this.camera);

    const axesHelper = new TH.AxesHelper(3);
    this.scene.add(axesHelper);
  }

  private createScene(): TH.Scene {
    const scene = new TH.Scene();

    const cube = new TH.Mesh(
      new TH.BoxGeometry(1, 1, 1),
      new TH.MeshBasicMaterial({ color: "red" }),
    );
    scene.add(cube);

    return scene;
  }

  run(): void {
    const controls = new OrbitControls(this.camera, this.canvas);
    controls.enableDamping = true;

    const render = () => {
      this.renderer.render(this.scene, this.camera);
      controls.update();
      window.requestAnimationFrame(render);
    };
    render();
  }
}

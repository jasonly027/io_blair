import { Environment, OrbitControls } from "@react-three/drei";
import { Canvas } from "@react-three/fiber";

export default function Game() {
  return (
    <div className="h-screen">
      <Canvas className="bg-red-200">
        <Environment background preset="lobby" />

        <pointLight position={[10, 10, 10]} decay={0} />
        <ambientLight intensity={0.2} />

        <OrbitControls />

        <axesHelper />

        <mesh rotation-x={Math.PI * 1.5}>
          <planeGeometry args={[5, 5, 5]} />
          <meshStandardMaterial color="green" />
        </mesh>
      </Canvas>
    </div>
  );
}

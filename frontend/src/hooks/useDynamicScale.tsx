import { SpringValue, useSpring } from "@react-spring/web";
import { useCallback, useMemo } from "react";

export interface DyanmicScaleValues {
  scale: { scale: SpringValue<number> };
  increaseScale: () => void;
  decreaseScale: () => void;
}

/**
 * Returns a dynamic scale value for animated HTML elements. Use increaseScale
 * to enlarge the element and decreaseScale to return it to its original scale.
 * 
 * @param scaleCeil The scale value to approach when increaseScale is called.
 */
export default function useDynamicScale(
  scaleCeil: number = 1.02,
): DyanmicScaleValues {
  const [scale, api] = useSpring(() => ({
    from: {
      scale: 1,
    },
  }));

  const increaseScale = useCallback(
    () => api.start({ to: { scale: scaleCeil } }),
    [api, scaleCeil],
  );

  const decreaseScale = useCallback(
    () => api.start({ to: { scale: 1 } }),
    [api],
  );

  return useMemo(
    () => ({
      scale,
      increaseScale,
      decreaseScale,
    }),
    [scale, increaseScale, decreaseScale],
  );
}

const mapUserDataTypes = ["oobSensor"] as const;
type MapUserDataType = (typeof mapUserDataTypes)[number];

export interface MapUserData {
  type: MapUserDataType;
}

export function isMapUserData(value: unknown): value is MapUserData {
  return (
    typeof value === "object" &&
    value !== null &&
    "type" in value &&
    mapUserDataTypes.includes(value.type as MapUserDataType)
  );
}

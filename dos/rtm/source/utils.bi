declare sub copymem (byval sourceSeg as integer, byval sourceOff as integer, byval destSeg as integer, byval destOff as integer, byval length as integer)
declare sub translatePolygons (byval polysInSeg as integer, byval polysInOff as integer, byval polysOutSeg as integer, byval polysOutOff as integer, byval x as integer, byval y as integer, byval z as integer, byval numPolies as integer)
declare sub rotatePolygons (byval polysSeg as integer, byval polysOff as integer, byval fixedCosine as integer, byval fixedSine as integer, byval numPolies as integer)
declare sub translateRotatePolygons (byval polysInSeg as integer, byval polysInOff as integer, byval polysOutSeg as integer, byval polysOutOff as integer, byval x as integer, byval y as integer, byval z as integer, byval fixedCosine as integer, byval fixedSine as integer, byval numPolies as integer)
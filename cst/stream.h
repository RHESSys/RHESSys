    /* Struct that holds info for streams that intersect with the current stream. */
    typedef struct {
        int streamId; //
        int row; //
        int col; //
    } streamIntxn;

    /* Stream intersections where no connected stream with a lower elevation was found. */
    typedef struct {
        int streamId;
        int row;
        int col;
    } unresolvedStreamIntxn ;

    /* This struct holds info about which zone, hill, and patches a stream crosses over. */
    typedef struct {
        int zoneId;
        int hillId;
        int patchId;
    } basinDivision;

    /* Struct that holds info for each stream */
    typedef struct {
        int streamId;
        float bottomWidth;	/* width of stream at outlet */
        float topWidth;		/* width of stream channel at head */
        double maxElevation;	/* max elevation of stream */
        double minElevation;	/* max elevation of stream */
        double adjacentStreamMinElevation;
        double maxHeight;	/* max channel height of stream, i.e. from LIDAR */
        float headXpos;		/* x location (easting) of stream head */
        float headYpos;		/* y location (northing) of stream head */
        float outletXpos;	/* x location (easting) of stream at outlet */
        float outletYpos;	/* y location (northin) of stream at outlet */
        float slope;
        float ManningsN;
        int   pixelCount;	/* count of pixels for this stream */
        int   intersectingUnitsCount;
        /* Intersections with other streams. Multiple upstream reaches are allowed but only one downstream reach is allowed.*/
        streamIntxn downstreamReach;
        streamIntxn *upstreamReaches;
        int upstreamCnt;
        basinDivision *basinDivisions;
        int basinDivisionCnt;
    } streamEntry ;

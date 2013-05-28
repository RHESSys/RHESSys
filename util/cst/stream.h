    /* Struct that holds info for streams that intersect with the current stream. */
    typedef struct {
        int streamId; //
        int row; //
        int col; //
        double minElevation;
        double maxElevation;
    } streamIntxn;

    /* This struct holds info about which zone, hill, and patches a stream crosses over. */
    typedef struct {
        int zoneId;
        int hillId;
        int patchId;
    } basinDivision;

    /* Struct that holds info for each stream */
    typedef struct {
        int streamId;
        float streamBottomWidth;	/* width of stream at bottom of channel */
        float streamTopWidth;		/* width of stream at top of channel */
        double maxElevation;	/* max elevation of stream */
        double minElevation;	/* max elevation of stream */
        double downstreamReachMinElevation;
        float slope;
        float streamDepth;
        float ManningsN;
        int   pixelCount;	/* count of pixels for this stream */
        int   intersectingUnitsCount;
        /* Intersections with other streams. Multiple upstream reaches are allowed but only one downstream reach is allowed.*/
        streamIntxn downstreamReach;
        streamIntxn *upstreamReaches;
        int upstreamCnt;
        basinDivision *basinDivisions;
        int basinDivisionCnt;
        bool printed;
    } streamEntry ;

// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <unordered_map>
#include <map>
#include <cmath>
#include <queue>

// Types for IDs
using StopID = long int;
using RegionID = std::string;
using RouteID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
RouteID const NO_ROUTE = "!!NO_ROUTE!!";
StopID const NO_STOP = -1;
RegionID const NO_REGION = "!!NO_REGION!!";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Infinity const int value (-10000 because of strange behaviour when adding to max int)
int const INF = std::numeric_limits<int>::max() - 10000;

// Type for Dijkstra, BFS and DFS algorithms' colour
using Colour = int;

// Type for time of day in minutes from midnight (i.e., 60*hours + minutes)
using Time = int;

// Type for a duration of time (in minutes)
using Duration = int;

// Type for a distance (in metres)
using Distance = int;

// Const values for colours
Colour const WHITE = 0;
Colour const GREY = 1;
Colour const BLACK = 2;

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Type for single stop's data, assisting declaration to enable declaration of Edge,
// since a StopData * is used in it
struct StopData;
struct TripEdge;

// Type for route graph edge
struct RouteEdge {
    RouteID route;
    StopData * nextStop;
    Distance distance;
    std::vector<TripEdge> trips = {};
    bool walking = false;
};

// Type for trip graph edge
struct TripEdge {
    Time departureTime;
    Time nextArrivalTime;
};

// Type for a single stop's data
struct StopData {
    StopID id;
    Name name;
    Coord coordinates;
    float distanceFromZero;
    RegionID region = NO_REGION;
    std::unordered_multimap<RouteID, RouteEdge> routesTo = {};

    // Variables for Dijkstra, A*, BFS and DFS-algorithms
    Colour colour = WHITE;              // Colour
    Distance d = INF;                        // Cumulative distance at the stop
    Distance de = INF;                       // A*-distance: journey so far + shortest possible distance
    StopData * pii = NULL;              // The stop where the trail comes from
    RouteID dRoute = NO_ROUTE;          // The route from which the trail comes from
    Time departureFromPrevious = INF;   // The departure time from previous stop
    Time arrivalTime = INF;             // The arrival time to the stop
};

// Type for a single region's data
struct RegionData {
    Name name;
    RegionID parentRegion = NO_REGION;
    std::vector<RegionID> childRegions = {};
    std::vector<StopID> regionStops = {};
};

// Type for a single routes data
struct RouteData {
    std::vector<StopID> stops = {};
    std::vector<StopData *> stopPtrs = {};
    std::vector<std::vector<Time>> trips = {};
};

// Type for stop-datastructure
using StopsDS = std::unordered_map<StopID, StopData>;

// Type for regions-datastructure
using RegionsDS = std::unordered_map<RegionID, RegionData>;

// Type for storing stop names
using StopNames = std::unordered_map<Name, std::vector<StopID>>;

// Type for storing all routes
using RoutesDS = std::unordered_map<RouteID, RouteData>;

// Type for Dijkstra priority queue pair
using PqPair = std::pair<int, StopData *>;

// Defining < and > operators for Dijkstra priority queue pair
inline bool operator<(PqPair p1, PqPair p2){
    return p1.first < p2.first;
}
inline bool operator>(PqPair p1, PqPair p2){
    return p1.first > p2.first;
}

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

inline bool operator<(Coord c1, Coord c2)
{
    float dist1 = sqrt (pow ( c1.x, 2) + pow ( c1.y,2));
    float dist2 = sqrt (pow ( c2.x, 2) + pow ( c2.y,2));

    if ( dist1 < dist2 ) { return true; }
    else if (dist1 == dist2 ) { return c1.y < c2.y; }
    else { return false; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Return value for cases where color was not found
Time const NO_TIME = std::numeric_limits<Time>::min();

// Return value for cases where Duration is unknown
Duration const NO_DURATION = NO_VALUE;

// Return value for cases where Duration is unknown
Distance const NO_DISTANCE = NO_VALUE;



// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: Θ(1)
    // Short rationale for estimate: .size-operation is a constant complexity operation
    int stop_count();

    // Estimate of performance: O(n*n)
    // Short rationale for estimate: .clear_routes() is the most unefficient part, more at it's rationale
    void clear_all();

    // Estimate of performance: Θ(n)
    // Short rationale for estimate: n (for-loop) times 1 (adding to vector's end is a constant)
    std::vector<StopID> all_stops();

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: adding to unordered map and finding values from it
    bool add_stop(StopID id, Name const& name, Coord xy);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: accessing unordered map value by find
    Name get_stop_name(StopID id);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: accessing unordered map value by find
    Coord get_stop_coord(StopID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance:  O (n log n)
    // Short rationale for estimate: For-loop and adding to a map concludes to O (n log n)
    std::vector<StopID> stops_alphabetically();

    // Estimate of performance: O (n log n)
    // Short rationale for estimate: For-loop and adding to a map concludes to O (n log n)
    std::vector<StopID> stops_coord_order();

    // Estimate of performance: Θ(1)
    // Short rationale for estimate: return datastructure private member
    StopID min_coord();

    // Estimate of performance: Θ(1)
    // Short rationale for estimate: return datastructure private member
    StopID max_coord();

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: find-method through stopNames-datastructure i.e. unordered map
    std::vector<StopID> find_stops(Name const& name);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: Find- and erase-methods through StopsDS-
    // and StopNames-datastructures i.e. unordered maps
    bool change_stop_name(StopID id, Name const& newname);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: Find-method through StopsDS-datastructure i.e. unordered map
    bool change_stop_coord(StopID id, Coord newcoord);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: adding to unordered map and finding values from it
    bool add_region(RegionID id, Name const& name);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: Find-method through StopsDS-datastructure i.e. unordered map
    Name get_region_name(RegionID id);

    // Estimate of performance: Θ(n)
    // Short rationale for estimate: n (for-loop) times 1 (adding to vector's end is a constant)
    std::vector<RegionID> all_regions();

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: Find-method through StopsDS- and RegionsDS-datastructures i.e. unordered maps
    bool add_stop_to_region(StopID id, RegionID parentid);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: Find-method through StopsDS- and RegionsDS-datastructures i.e. unordered maps
    bool add_subregion_to_region(RegionID id, RegionID parentid);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: While-loop through regions-datastructure, but usually the case is that there
    // is not many nested parents so usually is a constant
    std::vector<RegionID> stop_regions(StopID id);

    // Non-compulsory operations

    // Estimate of performance:
    // Short rationale for estimate:
    void creation_finished();

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: Loop through searchRegions-vector Θ(n) and
    // use find method within the loop O(log n)
    std::pair<Coord, Coord> region_bounding_box(RegionID id);

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: Loop through all stops and insert them to a map
    std::vector<StopID> stops_closest_to(StopID id);

    // Estimate of performance: O(n log n) ≈ Θ(1)
    // Short rationale for estimate: Worst case scenario is that we have to update min- or max-coordinate values
    // meaning that we call stops_coord_order()-method, but usually that's not the case. Otherwise we just find the value
    // from unordered_maps and remove the values
    bool remove_stop(StopID id);

    // Estimate of performance: O(n^2) ≈ Θ(1)
    // Short rationale for estimate: stop_region-methods used and find_first_of-method.
    // The asymptotic performance is quite bad (n^2), but on the other hand usually the n's
    // e.g. the region and parent regions of a single stop is not a large group, so the
    // real performance in this case is actually quite good, better than example performance
    // tests
    RegionID stops_common_region(StopID id1, StopID id2);

    // Phase 2 operations

    // Estimate of performance: Θ(n)
    // Short rationale for estimate: n (for-loop) times 1 (adding to vector's end is a constant)
    std::vector<RouteID> all_routes();

    // Estimate of performance: O(n*n) ≈ Θ(n)
    // Short rationale for estimate: n (for-loop) * O(n) ≈ Θ(1) (find-operation
    //                               from stops_-datastructure i.e. unordered map)
    bool add_route(RouteID id, std::vector<StopID> stops);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Looping through the vector<Edge> stopEdges
    std::vector<std::pair<RouteID, StopID>> routes_from(StopID stopid);

    // Estimate of performance: O(n) ≈ Θ(1)
    // Short rationale for estimate: Finding the route from unordered_map<RouteID, RouteData> routes_
    std::vector<StopID> route_stops(RouteID id);

    // Estimate of performance: O(n*n)
    // Short rationale for estimate: Looping through stops and clear()-method for each stop's routesTo
    //                               (unordered_map)
    void clear_routes();

    // Estimate of performance: O(V + E), where V=amount of stops, E=amount of connections
    // Short rationale for estimate: BFS algorithm, used journey_least_stops()
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_any(StopID fromstop, StopID tostop);

//    // Non-compulsory operations

    // Estimate of performance: O(V + E), where V=amount of stops, E=amount of connections
    // Short rationale for estimate: BFS algorithm
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_least_stops(StopID fromstop, StopID tostop);

    // Estimate of performance: O(V + E), where V=amount of stops, E=amount of connections
    // Short rationale for estimate: DFS algorithm
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_with_cycle(StopID fromstop);

    // Estimate of performance: O((V + E) * log V), where V=amount of stops, E=amount of routes
    // Short rationale for estimate: Moderately basic A* algorithm
    std::vector<std::tuple<StopID, RouteID, Distance>> journey_shortest_distance(StopID fromstop, StopID tostop);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Loop through route's stops (vector)
    bool add_trip(RouteID routeid, const std::vector<Time> &stop_times);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Loop through route's trips
    std::vector<std::pair<Time, Duration> > route_times_from(RouteID routeid, StopID stopid);

    // Estimate of performance: O((V + E) * log V), where V=amount of stops, E=amount of trips
    // Short rationale for estimate: Slightly modified Dijkstra's algorithm
    std::vector<std::tuple<StopID, RouteID, Time>> journey_earliest_arrival(StopID fromstop, StopID tostop, Time starttime);

    // Estimate of performance:
    // Short rationale for estimate:
    void add_walking_connections(); // Note! This method is completely optional, and not part of any testing

private:

    // Phase 1
    StopsDS stops_;
    StopID maxCoordStop_ = NO_STOP;
    StopID minCoordStop_ = NO_STOP;
    RegionsDS regions_;
    StopNames stopNames_;

    // Phase 2 additions
    RoutesDS routes_;

};

#endif // DATASTRUCTURES_HH

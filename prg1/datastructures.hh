// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <unordered_map>
#include <map>
#include <cmath>

// Types for IDs
using StopID = long int;
using RegionID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
StopID const NO_STOP = -1;
RegionID const NO_REGION = "!!NO_REGION!!";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Type for a single stop's data
struct StopData {
    Name name;
    Coord coordinates;
    float distanceFromZero;
    RegionID region = NO_REGION;
};

// Type for a single region's data
struct RegionData {
    Name name;
    RegionID parentRegion = NO_REGION;
    std::vector<RegionID> childRegions = {};
    std::vector<StopID> regionStops = {};
};

// Type for stop-datastructure
using StopsDS = std::unordered_map<StopID, StopData>;

// Type for regions-datastructure
using RegionsDS = std::unordered_map<RegionID, RegionData>;

// Type for storing stop names
using StopNames = std::unordered_map<Name, std::vector<StopID>>;

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


// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: Θ(1)
    // Short rationale for estimate: .size-operation is a constant complexity operation
    int stop_count();

    // Estimate of performance: Θ(n)
    // Short rationale for estimate: .clear() is a linear operation on unordered_map
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

private:

    StopsDS stops;
    StopID maxCoordStop = NO_STOP;
    StopID minCoordStop = NO_STOP;
    RegionsDS regions;
    StopNames stopNames;

};

#endif // DATASTRUCTURES_HH

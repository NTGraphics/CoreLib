//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//    .--------------------------------------------------.
//    |  This file is part of NTCodeBase                 |
//    |  Created 2018 by NT (https://ttnghia.github.io)  |
//    '--------------------------------------------------'
//                            \o/
//                             |
//                            / |
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <iostream>
#include <LibCommon/NeighborSearch/DataStructures.h>
#include <LibCommon/ParallelHelpers/ParallelObjects.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace NTCodeBase::NeighborSearch {
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template<Int N, class Real_t>
class NeighborSearch;

/**
 * @class PointSet.
 * Represents a set of points in three-dimensional space.
 */
template<Int N, class Real_t>
class PointSet {
public:
    PointSet(PointSet const& other) { *this = other; }
    PointSet& operator=(PointSet const& other) {
        m_x       = other.m_x;
        m_n       = other.m_n;
        m_dynamic = other.m_dynamic;

        m_neighbors = other.m_neighbors;
        m_keys      = other.m_keys;
        m_old_keys  = other.m_old_keys;

        m_sort_table = other.m_sort_table;

        return *this;
    }

    /**
     * Returns the number of neighbors of point i in the given point set.
     * @param i Point index.
     * @returns Number of points neighboring point i in point set point_set.
     */
    UInt n_neighbors(UInt point_set, UInt i) const {
        return static_cast<UInt>(m_neighbors[point_set][i].size());
    }

    /**
     * Fetches all neighbors of point i in the given point set.
     * @param point_set Point set index of other point set where neighbors have been searched.
     * @param i Point index for which the neighbors should be returned.
     * @returns Indices of neighboring point i in point set point_set.
     */
    const StdVT_UInt& neighbors(UInt point_set, UInt i) const {
        return m_neighbors[point_set][i];
    }

    /**
     * Fetches id pair of kth neighbor of point i in the given point set.
     * @param point_set Point set index of other point set where neighbors have been searched.
     * @param i Point index for which the neighbor id should be returned.
     * @param k Represents kth neighbor of point i.
     * @returns Index of neighboring point i in point set point_set.
     */
    UInt neighbor(UInt point_set, UInt i, UInt k) const {
        return m_neighbors[point_set][i][k];
    }

    /**
     * Returns the number of points contained in the point set.
     */
    UInt n_points() const { return m_n; }

    /*
     * Returns true, if the point locations may be updated by the user.
     **/
    bool is_dynamic() const { return m_dynamic; }

    /**
     * If true is passed, the point positions may be altered by the user.
     */
    void set_dynamic(bool v) { m_dynamic = v; }

    /**
     * Reorders an array according to a previously generated sort table by invocation of the method
     * "z_sort" of class "NeighborhoodSearch". Please note that the method "z_sort" of class
     * "Neighborhood search" has to be called beforehand.
     */
    template<class T>
    void sort_field(T* lst) const {
        if(m_sort_table.empty()) {
            std::cerr << "WARNING: No sort table was generated for the current point set. "
                      << "First invoke the method 'z_sort' of the class 'NeighborhoodSearch.'" << std::endl;
            return;
        }

        StdVT<T> tmp(lst, lst + m_sort_table.size());
        std::transform(m_sort_table.begin(), m_sort_table.end(),
#ifdef _MSC_VER
                       stdext::unchecked_array_iterator<T*>(lst),
#else
                       lst,
#endif
                       [&](int i) { return tmp[i]; });
    }

private:
    friend NeighborSearch<N, Real_t>;
    PointSet(const Real_t* x, UInt n, bool dynamic)
        : m_x(x), m_n(n), m_dynamic(dynamic), m_neighbors(n) {
        resize_keys(n);
    }

    void resize(const Real_t* x, UInt n) {
        m_x = x;
        m_n = n;
        resize_keys(n);
        m_neighbors.resize(n);
    }

    void resize_keys(UInt n) {
        if constexpr(N == 2) {
            m_keys.resize(n, {
                    std::numeric_limits<int>::lowest(),
                    std::numeric_limits<int>::lowest()
                });
            m_old_keys.resize(n, {
                    std::numeric_limits<int>::lowest(),
                    std::numeric_limits<int>::lowest()
                });
        } else {
            m_keys.resize(n, {
                    std::numeric_limits<int>::lowest(),
                    std::numeric_limits<int>::lowest(),
                    std::numeric_limits<int>::lowest()
                });
            m_old_keys.resize(n, {
                    std::numeric_limits<int>::lowest(),
                    std::numeric_limits<int>::lowest(),
                    std::numeric_limits<int>::lowest()
                });
        }
    }

    const Real_t* point(UInt i) const {
        if constexpr(N == 2) {
            return &m_x[2 * i];
        } else {
            return &m_x[3 * i];
        }
    }

private:

    const Real_t* m_x;
    UInt          m_n;
    bool          m_dynamic;

    StdVT<HashKey<N>> m_keys, m_old_keys;
    StdVT_UInt        m_sort_table;

    StdVT<StdVT<StdVT_UInt>>                m_neighbors;
    StdVT<StdVT<ParallelObjects::SpinLock>> m_locks;
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
} // end namespace NTCodeBase::NeighborSearch

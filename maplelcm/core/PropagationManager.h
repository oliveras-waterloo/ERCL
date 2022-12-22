/****************************************************************************************[Solver.h]
MiniSat -- Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
           Copyright (c) 2007-2010, Niklas Sorensson

Chanseok Oh's MiniSat Patch Series -- Copyright (c) 2015, Chanseok Oh
 
Maple_LCM, Based on MapleCOMSPS_DRUP -- Copyright (c) 2017, Mao Luo, Chu-Min LI, Fan Xiao: implementing a learnt clause minimisation approach
Reference: M. Luo, C.-M. Li, F. Xiao, F. Manya, and Z. L. , “An effective learnt clause minimization approach for cdcl sat solvers,” in IJCAI-2017, 2017, pp. to–appear.

Maple_LCM_Dist, Based on Maple_LCM -- Copyright (c) 2017, Fan Xiao, Chu-Min LI, Mao Luo: using a new branching heuristic called Distance at the beginning of search 

xMaple_LCM_Dist, based on Maple_LCM_Dist -- Copyright (c) 2022, Jonathan Chung, Vijay Ganesh, Sam Buss

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef Minisat_PropagationManager_h
#define Minisat_PropagationManager_h

#include <core/SolverTypes.h>
#include <mtl/Heap.h>
#include <map>

// Making some internal methods visible for testing
#ifdef TESTING
#define protected public
#endif

namespace Minisat {
    // Forward declarations
    class Solver;

    class PropagationManager {
    protected:

        // Comparator for BCP priority queue
        template<class T>
        struct LitOrderLt {
            const vec<T>&  activity;
            bool operator () (Var x, Var y) const {
                x >>= 1; y >>= 1;
                return activity[x] > activity[y];
            }
            LitOrderLt(const vec<T>&  act) : activity(act) { }
        };

        /////////////////////////
        // Convenience methods //
        /////////////////////////
        // Call the methods implemented by Solver.h by the same names
        // Implementation is overridden in unit testing

        inline lbool value(Var x) const; // Gets the truth assignment of a variable
        inline lbool value(Lit p) const; // Gets the truth assignment of a literal

        ////////////////////
        // HELPER METHODS //
        ////////////////////

#ifdef TESTING
        inline void set_value(Var x, lbool v, int l);
#endif

        lbool bcpValue  (Var x) const; // The queued value of a variable.
        lbool bcpValue  (Lit p) const; // The queued value of a literal.

        /**
         * @brief Perform all propagations for a single literal
         * 
         * @param p the literal to propagate
         * @return The conflicting clause if a conflict arises, otherwise CRef_Undef.
         */
        CRef propagate_single(Lit p);

        /**
         * @brief Relocate watcher CRefs to new ClauseAllocator
         * 
         * @param ws The watchers for which to relocate CRefs
         * @param to The ClauseAllocator into which to reloc 
         */
        void relocWatchers(vec<Watcher>& ws, ClauseAllocator& to);

        //////////////////////
        // MEMBER VARIABLES //
        //////////////////////

        // 'watches[lit]' is a list of constraints watching 'lit' (will go there if literal becomes true).
        OccLists<Lit, vec<Watcher>, WatcherDeleted> watches_bin; // Watches for binary clauses
        OccLists<Lit, vec<Watcher>, WatcherDeleted> watches;     // Watches for non-binary clauses

        Heap< LitOrderLt<double> > bcp_order_heap; // BCP priority queue
        vec<lbool> bcp_assigns;

        ClauseAllocator& ca;
        Solver* solver;

#ifdef TESTING
        std::map< Var, std::pair<lbool, int> > test_value;
#endif

    public:
        /**
         * @brief Construct a new PropagationManager object
         * 
         * @param s Pointer to main solver object - must not be nullptr
         */
        PropagationManager(Solver* s);

        /**
         * @brief Destroy the PropagationManager object
         */
        ~PropagationManager();

        /**
         * @brief Register watchers for a new variable
         * 
         * @param v the variable to register
         * @note Assumes that @code{v} has not already been registered
         */
        void newVar(Var v);

        /**
         * @brief Attach a clause to watcher lists.
         * 
         * @param c The clause to attach.
         * @param cr The CRef of the clause to attach. Must match @code{c}.
         * 
         * @note c and cr are provided separately to enable compiler optimization at the caller.
         */
        void attachClause(const Clause& c, CRef cr);

        /**
         * @brief Attach a clause to watcher lists.
         * 
         * @param cr The CRef of the clause to attach.
         */
        void attachClause(CRef cr);

        /**
         * @brief Detach a clause from watcher lists.
         * 
         * @param c The clause to detach.
         * @param cr The CRef of the clause to detach. Must match @code{c}.
         * @param strict False to use lazy detaching, true otherwise
         * 
         * @note c and cr are provided separately to enable compiler optimization at the caller.
         */
        void detachClause(const Clause& c, CRef cr, bool strict);

        /**
         * @brief Detach a clause from watcher lists.
         * 
         * @param cr The CRef of the clause to detach.
         * @param strict False to use lazy detaching, true otherwise
         */
        void detachClause(CRef cr, bool strict);

        /**
         * @brief Relocate CRefs to new ClauseAllocator
         * 
         * @param to The ClauseAllocator into which to reloc 
         */
        void relocAll(ClauseAllocator& to);

        /**
         * @brief Propagate all enqueued facts.
         * 
         * @return The conflicting clause if a conflict arises, otherwise CRef_Undef. 
         */
        CRef propagate();

        /**
         * @brief Simplified implementation of @code{propagate} for simplifying learnt clauses.
         * 
         * @return The conflicting clause if a conflict arises, otherwise CRef_Undef.  
         */
        CRef simplePropagate();

        /**
         * @brief Decrease the BCP priority of a given variable
         * 
         * @param v the variable to prioritize
         */
        void decreasePriority(Var v);

        /**
         * @brief Increase the BCP priority of a given variable
         * 
         * @param v the variable to prioritize
         */
        void increasePriority(Var v);

        /**
         * @brief Get list of non-binary clause watchers for a given literal
         * 
         * @param l the literal whose watchers should be returned
         * @return non-binary clause watchers for @code{l}
         */
        const vec<Watcher>& getWatchers(Lit l) const;

        /**
         * @brief Get list of binary clause watchers for a given literal
         * 
         * @param l the literal whose watchers should be returned
         * @return binary clause watchers for @code{l}
         */
        const vec<Watcher>& getBinaryWatchers(Lit l) const;

        /**
         * @brief Move undefined literal to index 0, ensuring that watcher invariants are satisfied
         * 
         * @param cr The CRef of the asserting clause
         * @param i_undef The index of the undefined literal in the clause
         * @param i_max The index of the literal in the clause with the highest decision level
         */
        void enforceWatcherInvariant(CRef cr, int i_undef, int i_max);
    };

    // Explicitly instantiate required templates
    template class PropagationManager::LitOrderLt<double>;

    inline lbool PropagationManager::bcpValue(Var x) const { return bcp_assigns[x]; }
    inline lbool PropagationManager::bcpValue(Lit p) const { return bcp_assigns[var(p)] ^ sign(p); }

    inline void PropagationManager::newVar(Var v) {
        watches_bin.init(mkLit(v, false));
        watches_bin.init(mkLit(v, true ));
        watches    .init(mkLit(v, false));
        watches    .init(mkLit(v, true ));
        bcp_assigns.push(l_Undef);
    }

    inline void PropagationManager::attachClause(const Clause& c, CRef cr) {
        assert(c.size() > 1);
        OccLists<Lit, vec<Watcher>, WatcherDeleted>& ws = c.size() == 2 ? watches_bin : watches;
        ws[~c[0]].push(Watcher(cr, c[1]));
        ws[~c[1]].push(Watcher(cr, c[0]));
    }
    inline void PropagationManager::attachClause(CRef cr) { attachClause(ca[cr], cr); }

    inline void PropagationManager::detachClause(const Clause& c, CRef cr, bool strict) {
        assert(c.size() > 1);
        OccLists<Lit, vec<Watcher>, WatcherDeleted>& ws = c.size() == 2 ? watches_bin : watches;
        if (strict) {
            remove(ws[~c[0]], Watcher(cr, c[1]));
            remove(ws[~c[1]], Watcher(cr, c[0]));
        } else {
            // Lazy detaching: (NOTE! Must clean all watcher lists before garbage collecting this clause)
            ws.smudge(~c[0]);
            ws.smudge(~c[1]);
        }
    }
    inline void PropagationManager::detachClause(CRef cr, bool strict) { detachClause(ca[cr], cr, strict); }

    inline void PropagationManager::relocWatchers(vec<Watcher>& ws, ClauseAllocator& to) {
        for (int i = 0; i < ws.size(); i++) ca.reloc(ws[i].cref, to);
    }

    inline void PropagationManager::decreasePriority(Var v) {
        // Note: this uses the increase() method because we're using a min-heap as a max-heap
        Lit l = mkLit(v);
        if (bcp_order_heap.inHeap(( l).x)) bcp_order_heap.increase(( l).x);
        if (bcp_order_heap.inHeap((~l).x)) bcp_order_heap.increase((~l).x);
    }

    inline void PropagationManager::increasePriority(Var v) {
        // Note: this uses the decrease() method because we're using a min-heap as a max-heap
        Lit l = mkLit(v);
        if (bcp_order_heap.inHeap(( l).x)) bcp_order_heap.decrease(( l).x);
        if (bcp_order_heap.inHeap((~l).x)) bcp_order_heap.decrease((~l).x);
    }

    inline const vec<Watcher>& PropagationManager::getWatchers      (Lit l) const { return watches    [l]; }
    inline const vec<Watcher>& PropagationManager::getBinaryWatchers(Lit l) const { return watches_bin[l]; }

} // namespace Minisat

#endif
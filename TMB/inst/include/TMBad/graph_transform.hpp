#ifndef HAVE_GRAPH_TRANSFORM_HPP
#define HAVE_GRAPH_TRANSFORM_HPP
// Autogenerated - do not edit by hand !
#define GLOBAL_HASH_TYPE unsigned int
#define GLOBAL_COMPRESS_TOL 16
#define GLOBAL_UNION_OR_STRUCT union
#define stringify(s) #s
#define xstringify(s) stringify(s)
#define THREAD_NUM 0
#define GLOBAL_INDEX_VECTOR std::vector<GLOBAL_INDEX_TYPE>
#define GLOBAL_INDEX_TYPE unsigned int
#define CONSTEXPR constexpr
#define ASSERT2(x, msg)                          \
  if (!(x)) {                                    \
    Rcerr << "ASSERTION FAILED: " << #x << "\n"; \
    Rcerr << "POSSIBLE REASON: " << msg << "\n"; \
    abort();                                     \
  }
#define GLOBAL_MAX_NUM_THREADS 48
#define INDEX_OVERFLOW(x) \
  ((size_t)(x) >= (size_t)std::numeric_limits<GLOBAL_INDEX_TYPE>::max())
#define ASSERT(x)                                \
  if (!(x)) {                                    \
    Rcerr << "ASSERTION FAILED: " << #x << "\n"; \
    abort();                                     \
  }
#define GLOBAL_REPLAY_TYPE ad_aug
#define GLOBAL_MIN_PERIOD_REP 10
#define INHERIT_CTOR(A, B)                                       \
  A() {}                                                         \
  template <class T1>                                            \
  A(const T1 &x1) : B(x1) {}                                     \
  template <class T1, class T2>                                  \
  A(const T1 &x1, const T2 &x2) : B(x1, x2) {}                   \
  template <class T1, class T2, class T3>                        \
  A(const T1 &x1, const T2 &x2, const T3 &x3) : B(x1, x2, x3) {} \
  template <class T1, class T2, class T3, class T4>              \
  A(const T1 &x1, const T2 &x2, const T3 &x3, const T4 &x4)      \
      : B(x1, x2, x3, x4) {}
#define GLOBAL_SCALAR_TYPE double
#include <cstring>
#include <list>
#include <map>
#include "checkpoint.hpp"
#include "global.hpp"
#include "integrate.hpp"
#include "radix.hpp"

namespace TMBad {

/** \brief Match x vector in y vector
    \return Logical vector of the same length as x
    \note Intended for small vectors only
*/
template <class T>
std::vector<bool> lmatch(const std::vector<T> &x, const std::vector<T> &y) {
  std::vector<bool> ans(x.size(), false);
  for (size_t i = 0; i < x.size(); i++)
    for (size_t j = 0; j < y.size(); j++) ans[i] = ans[i] || (x[i] == y[j]);
  return ans;
}

/** \brief Convert logical vector to index vector */
template <class I>
std::vector<I> which(const std::vector<bool> &x) {
  std::vector<I> y;
  for (size_t i = 0; i < x.size(); i++)
    if (x[i]) y.push_back(i);
  return y;
}

/** \brief Convert logical vector to index vector */
std::vector<size_t> which(const std::vector<bool> &x);

/** \brief Vector subset by boolean mask */
template <class T>
std::vector<T> subset(const std::vector<T> &x, const std::vector<bool> &y) {
  ASSERT(x.size() == y.size());
  std::vector<T> ans;
  for (size_t i = 0; i < x.size(); i++)
    if (y[i]) ans.push_back(x[i]);
  return ans;
}

/** \brief Vector subset by index vector */
template <class T, class I>
std::vector<T> subset(const std::vector<T> &x, const std::vector<I> &ind) {
  std::vector<T> ans(ind.size());
  for (size_t i = 0; i < ind.size(); i++) ans[i] = x[ind[i]];
  return ans;
}

/** \brief Make space for new elements in an existing vector
    \details Resize and move elements of a vector to make space for
    index `i` new members.
    Vector `x` and index vector `i` are modified such that

    - Index vector points at the same elements as before:
      `x_new[i_new] == x_old[i_old]`
      except that `i_new` is sorted while `i_old` might not be.
    - Undefined elements are at positions `i-1`
    - Removing the undefined elements you get the old vector
*/
template <class T, class I>
void make_space_inplace(std::vector<T> &x, std::vector<I> &i, T space = T(0)) {
  std::vector<bool> mark(x.size(), false);
  for (size_t k = 0; k < i.size(); k++) {
    ASSERT(!mark[i[k]]);
    mark[i[k]] = true;
  }
  std::vector<T> x_new;
  std::vector<I> i_new;
  for (size_t k = 0; k < x.size(); k++) {
    if (mark[k]) {
      x_new.push_back(space);
      i_new.push_back(x_new.size());
    }
    x_new.push_back(x[k]);
  }
  std::swap(x, x_new);
  std::swap(i, i_new);
}

/** \brief Inverse permutation */
template <class T>
std::vector<T> invperm(const std::vector<T> &perm) {
  std::vector<T> iperm(perm.size());
  for (size_t i = 0; i < perm.size(); i++) iperm[perm[i]] = i;
  return iperm;
}

/** \brief Match x vector in y vector */
template <class T>
std::vector<size_t> match(const std::vector<T> &x, const std::vector<T> &y) {
  return which(lmatch(x, y));
}

/** \brief Integer product function */
size_t prod_int(const std::vector<size_t> &x);

/** \brief Get permutation that sorts a vector

    If `x` equals

    `{5, 4, 3, 5, 4, 3, 5, 2, 1}`

    Then `order(x)` is

    `{8, 7, 2, 5, 1, 4, 0, 3, 6}`

    \note Order of duplicates within groups is preserved
*/
template <class T>
std::vector<size_t> order(std::vector<T> x) {
  std::vector<std::pair<T, size_t> > y(x.size());
  for (size_t i = 0; i < x.size(); i++) {
    y[i].first = x[i];
    y[i].second = i;
  }
  sort_inplace(y);
  std::vector<size_t> z(x.size());
  for (size_t i = 0; i < x.size(); i++) {
    z[i] = y[i].second;
  }
  return z;
}

/** \brief Find boundary without using the graph  */
std::vector<bool> reverse_boundary(global &glob, const std::vector<bool> &vars);

/** \brief Get node indices of the accumulation tree or its boundary

    The tree is defined as the maximal **topologically closed** sub
    graph T, containing only linear operators.
    In other words, the nodes in the tree are not allowed to produce
    variables that are inputs to any non linear operators.
*/
std::vector<Index> get_accumulation_tree(global &glob, bool boundary = false);

/** \brief Find nodes by name */
std::vector<Index> find_op_by_name(global &glob, const char *name);

/** \brief substitute node index sequence by independent variables
    \note Currently all operators in sequence must be different from InvOp
*/
std::vector<Index> substitute(global &glob, const std::vector<Index> &seq,
                              bool inv_tags = true, bool dep_tags = true);

/** \brief substitute by op_name */
std::vector<Index> substitute(global &glob, const char *name,
                              bool inv_tags = true, bool dep_tags = true);

/** \brief Split a computational graph by it's accumulation tree
    \details This routine transforms a function object with univariate
    output into a function object with multiple outputs. Each new
    output represents a term of the original function, i.e. by summing
    all the terms one can recover the original function.
    \note FIXME: Handle the case Range() > 1. Select which component ?
*/
global accumulation_tree_split(global glob, bool sum_ = false);

/** \brief Reduce a multivariate output function by summing its range components
    \details
    On input  the tape represents a function f:R^m->R^n.
    On output the tape represents a function f:R^m->R.
    \param glob Tape which is modified by this function
*/
void aggregate(global &glob, int sign = 1);

/** \brief Read the current tape's state and restore it on request
    \details Useful when a tape needs minor temporary modification and
    at some point must be restored to its original state.
*/
struct old_state {
  std::vector<Index> dep_index;
  size_t opstack_size;
  global &glob;
  old_state(global &glob);
  void restore();
};

std::vector<Index> remap_identical_sub_expressions(
    global &glob, std::vector<Index> inv_remap);
struct term_info {
  global &glob;
  std::vector<Index> id;
  std::vector<size_t> count;
  term_info(global &glob, bool do_init = true);
  void initialize(std::vector<Index> inv_remap = std::vector<Index>(0));
};

template <class Float = ad_adapt>
struct logIntegrate_t {
  typedef Float Scalar;
  global glob;
  double mu, sigma, f_mu;
  double f(double x) {
    Index k = glob.inv_index.size();
    glob.value_inv(k - 1) = x;
    glob.forward();
    return glob.value_dep(0);
  }
  double g(double x) { return f(x + .5) - f(x - .5); }
  double h(double x) { return g(x + .5) - g(x - .5); }
  /** \brief Rescale integrand
      \f[
      \log\left(\int_{-\infty}^{\infty} \exp(f(x))\:dx\right)=
      \log\left(\int_{-\infty}^{\infty} \exp(f(\sigma u +
     \mu)-f(\mu))\:du\right) + \log(\sigma) + f(\mu) \f]
  */
  void rescale_integrand(const std::vector<ad_plain> &x) {
    for (size_t i = 0; i < x.size(); i++) glob.value_inv(i) = x[i].Value();
    f_mu = f(mu);
    for (int i = 0; i < 100; i++) {
      double mu_new = mu - g(mu) / h(mu);
      double f_mu_new = f(mu_new);
      if (f_mu_new - f_mu > 1) {
        mu = mu_new;
        f_mu = f_mu_new;
      } else {
        break;
      }
    }
    sigma = 1. / sqrt(std::max(-h(mu), 1e-8));
  }

  logIntegrate_t(global &glob) : glob(glob), mu(0), sigma(1), f_mu(0) {
    ASSERT(glob.dep_index.size() == 1);
  }
  logIntegrate_t() {}
  global::replay *p_replay;

  Float operator()(Float u) {
    Index k = glob.inv_index.size();
    p_replay->value_inv(k - 1) = sigma * u + mu;
    p_replay->forward(false, false);
    Float ans = exp(p_replay->value_dep(0) - f_mu);
    if (ans == INFINITY) ans = INFINITY;
    return ans;
  }

  std::vector<ad_plain> operator()(const std::vector<ad_plain> &x) {
    rescale_integrand(x);
    global::replay replay(this->glob, *get_glob());
    p_replay = &replay;
    replay.start();
    Index k = glob.inv_index.size();
    for (Index i = 0; i < k - 1; i++) replay.value_inv(i) = x[i];
    Float I = integrate(*this);
    if (I == INFINITY) I = 0;
    Float ans = log(I) + log(sigma) + f_mu;
    replay.stop();
    return std::vector<ad_plain>(1, ans);
  }
};

struct integrate_subgraph {
  global &glob;
  std::vector<Index> random;
  graph forward_graph;
  graph reverse_graph;
  std::vector<Index> var_remap;
  std::vector<bool> mark;
  bool adaptive;
  /** \brief CTOR of adaptive integrated subgraph
      \param glob Output from `accumulation_tree_split()`.
  */
  integrate_subgraph(global &glob, std::vector<Index> random,
                     bool adaptive = false);
  /** \brief Attempt to integrate i'th independent variable
      \param i Integrate `inv_index[i]`.
  */
  global &try_integrate_variable(Index i);
  global &gk();
};

/** \brief Utilility class for `sequential_reduction`.

    Helps looping through a multivariate index along any possible sub slice.

    Example: Loop through `{0,1}^5` with middle index (2) slowest running and
   remaining indices (0,1,3,4) fastest running. \code #include <TMBad.hpp> using
   namespace TMBad; int main() { multivariate_index x(2, 5, false); x.mask(2) =
   true; for (int j=0; j<x.count(); ++j, ++x) { x.flip(); for (int i=0;
   i<x.count(); ++i, ++x) { Rcout << x.index() << " pointer=" << x << "\n";
        }
        x.flip();
      }
    }
    \endcode
*/
struct multivariate_index {
 private:
  std::vector<size_t> x;
  std::vector<bool> mask_;
  size_t pointer;
  std::vector<size_t> bound;

 public:
  /** \brief Number of elements indexed by this `multivariate_index`
      \note It is generally recommended to avoid re-calculating the
      `count` inside a loop such as `for(i=0; i<x.count(); i++, ++x)`.
  */
  size_t count();
  /** \brief CTOR of `multivariate_index` representing `{0,..,B-1}^D`
      \param bound B
      \param dim D
      \param flag Fill mask of active dimensions with this value
  */
  multivariate_index(size_t bound_, size_t dim, bool flag = true);
  /** \brief CTOR of `multivariate_index` representing the product set `Prod_i
     {0,..,B[i]-1}` \param Bound vector B containing the number of indices for
     each dimension \param flag Fill mask of active dimensions with this value
  */
  multivariate_index(std::vector<size_t> bound, bool flag = true);
  /** \brief Flip the mask of active dimensions  */
  void flip();
  /** \brief Advance to next element of this sub slice */
  multivariate_index &operator++();
  /** \brief Get univariate pointer */
  operator size_t();
  /** \brief Get given component of this `multivariate_index` */
  size_t index(size_t i);
  /** \brief Get `multivariate_index` as a vector */
  std::vector<size_t> index();
  /** \brief Get / set active dimensions of this `multivariate_index` */
  std::vector<bool>::reference mask(size_t i);
  /** \brief Set all active dimensions of this `multivariate_index` */
  void set_mask(const std::vector<bool> &mask);
};

/** \brief Utilility class for `sequential_reduction`.

    Class to represent a clique, i.e. a small set of variables for
    which the joint probability must be fully tabulated.
*/
struct clique {
  /** \brief Variable indices of this clique */
  std::vector<Index> indices;
  /** \brief Log-probabilites of this clique */
  std::vector<ad_aug> logsum;
  /** \brief Dimension of logsum array */
  std::vector<size_t> dim;
  size_t clique_size();
  clique();
  void subset_inplace(const std::vector<bool> &mask);
  void logsum_init();
  bool empty() const;
  bool contains(Index i);
  /** \brief Determine array offsets and stride of this clique
      \details Recall that `indices` are the sorted indices of this
      clique and `logsum` is the value array of this clique. Also,
      recall that the value array is stored with the smallest index
      fastest running and largest index slowest running.
      Assume that `super` is a larger clique than this one, so that
      this clique can be embedded into the super clique. Also, assume
      that both cliques contain a common index `i` to be integrated.
      The routine `get_stride()` locates the integration slice in
      `this` array for any combination of super indices *not* to be
      integrated.
      \param super 'Super clique' containing this clique's indices as a subset.
      \param i Index to integrate out of the super clique.
      \param offset *Output:* Offset pointers into this array, one for each
     combination of super indices except i. \param stride *Output:* Stride of
     the integration index (i) within this array.
  */
  void get_stride(const clique &super, Index ind, std::vector<ad_plain> &offset,
                  Index &stride);
};

/** \brief Utilility class for `sequential_reduction`.

    Class to represent a grid, i.e. x and w vectors used to approximate

    \[\int f(x)\:dx = \sum_i f(x_i)w_i\]
*/
struct sr_grid {
  std::vector<Scalar> x;
  std::vector<Scalar> w;
  sr_grid();

  sr_grid(Scalar a, Scalar b, size_t n);

  sr_grid(size_t n);
  size_t size();

  std::vector<ad_plain> logw;
  ad_plain logw_offset();
};

/** \brief Sequential reduction algorithm

    - Integrated terms are functions of cliques.
    - There can be arbitrarily many active cliques.
    - A clique must be represented by a sorted index vector (the
      variables) and a 'logsum' vector of the same length as
      length(grid)^length(clique) (can be very long !).
    - Everytime we process a summation we look right for all the
      cliques. We must find all the cliques that contain the variable
      we are currently integrating. All matching cliques must be
      updated: we remove the current integration variable and add it's
      parents. It may be that no more terms depend on the current
      integration variable (the forward/backward graph has no dep
      variables). In this case we simply sum out the variable and
      remove it from all the cliques. Eventually all cliques will
      become empty.
    - If at anytime two cliques have the same index vector they can
      be merged into one. However, this optimization is not
      necessary for the method to work.
    - The final node xend to be integrated will in general belong to a
      number of identical cliques {xend}, {xend},... The sum of all
      those cliques is our end result.

  Example: Consider the circular graph

  ```
  +-x1-x2-x3-x4-x5-x6-x7-+
  |______________________|
  ```

  Each edge is a logspace term. Assume we integrate in some random order:

  ```
  x1, x5, x7, x6, ...
  ```

  - When integrating wrt x1 we generate the clique {2, 7}.
  - When integrating wrt x5 we generate the clique {4, 6}.
  - When integrating wrt x7 we notice that 7 is already in a clique. There is
  only one term (edge) left that depends on x7: f(x6,x7).
  - This term is tabulated and log-integrated versus {2, 7} to produce a new
  clique {2, 6}.
  - When integrating wrt x6 we notice that 6 is already in two cliques. There
  are no terms (edges) left that depend on x6.
  - Therefore x6 is summed out of the two cliques. The updated cliques are {4}
  and {2}.
*/
struct sequential_reduction {
  std::list<clique> cliques;
  std::vector<sr_grid> grid;
  std::vector<Index> inv2grid;
  global &glob;
  global new_glob;
  std::vector<Index> random;
  global::replay replay;
  std::vector<bool> mark;
  graph forward_graph;
  graph reverse_graph;
  std::vector<Index> var_remap;
  const static Index NA = -1;
  std::vector<Index> op2inv_idx;
  std::vector<Index> op2dep_idx;
  std::vector<bool> terms_done;
  term_info tinfo;
  std::map<size_t, std::vector<ad_aug> > cache;
  /** \brief CTOR of sequential reduction object
      \details The optimal order is not found by this routine. In
      general a good choice is the fill-reducing permutation of the
      sparse Cholesky factorization. For hierarchical models the
      optimal ordering is the opposite of the 'natural simulation
      order'.
      \param glob Output from `accumulation_tree_split()`.
      \param random Which independent variables to integrate out and in what
     order (if `perm=false`). To be specific, the order of integration is
     `inv_index[random[0]]`, `inv_index[random[1]]`, ... \param grid Vector of
     grids to be used by individual random effects - see `random2grid`. \param
     random2grid Factor determining the grid to be used for each random effect.
     To be specific, the i'th random effect `random[i]` uses the grid
     `grid[random2grid[i]]`. \param perm Apply a permutation of `random` based
     on a simple heuristic (see `reorder_random()`).
  */
  sequential_reduction(global &glob, std::vector<Index> random,
                       std::vector<sr_grid> grid =
                           std::vector<sr_grid>(1, sr_grid(-20, 20, 200)),
                       std::vector<Index> random2grid = std::vector<Index>(0),
                       bool perm = true);
  /** \brief Re-order random effects

     Two random effects are *connected* if they both affect the same
     term. This function finds a fill-reducing permutation of the
     corresponding un-directed graph.
  */
  void reorder_random();

  std::vector<size_t> get_grid_bounds(std::vector<Index> inv_index);

  std::vector<sr_grid *> get_grid(std::vector<Index> inv_index);
  /** \brief tabulate each combination of variables of a subgraph

      It is assumed that a subgraph has been made. The subgraph has a certain
     number of inputs and outputs. This function tabulates *a given output*
     `dep_index` for *all combinations of inputs* `inv_index`. To avoid
     tabulating identical functions many times, we a use caching based on term
     hash codes. \note If multiple grids are in play we have to include the grid
      ids in the hash codes! (although it is hard to imagine a case
      where one would wish to tabulate identical functions using
      different grids).
  */
  std::vector<ad_aug> tabulate(std::vector<Index> inv_index, Index dep_index);

  /** \brief Merge all cliques that contain a given independent variable
      \param i Index to remove

      Example:
      ```
      Assume i=2 and the current state contains cliques
      (1) (1 2 3) (2 4) (1 3)
      We start by finding all cliques that contain 2, in this case:
      (1 2 3) (2 4)
      The 'super' clique becomes
      (1 2 3 4)
      Rather than expanding this as a 4D array we figure out how to
      access the 4D information from the existing lower dimensional
      arrays (1 2 3) and (2 4), i.e. 'offsets' and 'strides'.
      We integrate 'i' out of the 4D array without constructing the 4D
      array. The resulting clique is
      (1 3 4)
      This new (merged) clique is added to the list and the updated list now
     looks like: (1) (1 3 4) (1 3)
      ```
  */
  void merge(Index i);

  /** \brief Integrate independent variable number i, (`inv_index[i]`).
      \details The algorithm works as follows:
      1. Identify all factors (terms in logspace) that depend on x_i **and**
     have not yet been integrated.
      2. Identify all variables V that affect these terms (for now assume all
     variables are random effects) by determining a reverse sub-graph.
      3. Based on the sub-graph tabulate a new clique C consisting of V indices.
      4. In the set S of all existing cliques locate those containing i. Merge
     them with C while removing them from S.
      5. Integrate variable i out of C and add C to S.
  */
  void update(Index i);
  void show_cliques();
  void update_all();
  ad_aug get_result();
  global marginal();
};

/** \brief Split a computational graph using a simple heuristic

    \details Given a function object representing a mapping
    `f:R^n->R^m`.  Assign each of the `m` outputs to threads and split
    the function `f` into `m` new functions that can be evaluated in
    parallel.

    Let `dep(1), ...,dep(m)` denote the dependendent variables and let
    `T(dep(i))` be the reverse tree starting from output node
    `dep(i)`.

    For some given ordering of the dependent variables
    `dep(1),...,dep(n)` we define the work

    `Work(dep(i)) = |T(dep(i))|`

    as the size of the i'th sub tree and

    `dWork(dep(i)) = | T(dep(i)) \ Union_(j<i) T(dep(j)) |`

    i.e. the amount of extra work to calculate `dep(i)` assuming that
    all previous `dep(j)` have been calculated.

    The idea beind the algorithm is to sort the work in decreasing
    order and assign dependent variables to threads as follows:

    - If `dWork(i)` is 'small' assign `dep(i)` to current thread
    - Otherwise, switch to the thread with the smallest assigned work

    However, we approximate `|Work(dep(i))|` by the maximum tree depth of
    `T(dep(i))` (because it's easier to compute).  The assigned work
    by thread is also replaced by a crude approximation.
*/
struct autopar {
  global &glob;
  graph reverse_graph;
  size_t num_threads;
  /** \brief Sum up result for each thread? */
  bool do_aggregate;
  /** \brief Keep all independent variables for each thread? */
  bool keep_all_inv;
  /** \brief Complete subgraphs by thread */
  std::vector<std::vector<Index> > node_split;
  /** \brief Result: Pointers into original independent variables */
  std::vector<std::vector<Index> > inv_idx;
  /** \brief Result: Pointers into original dependent variables */
  std::vector<std::vector<Index> > dep_idx;
  /** \brief Result: Vector of computational graphs */
  std::vector<global> vglob;
  autopar(global &glob, size_t num_threads);
  /** \brief Give an estimate (maximum tree depth) of the size of each
      reverse sub tree. */
  std::vector<size_t> max_tree_depth();

  template <class T>
  size_t which_min(const std::vector<T> &x) {
    return std::min_element(x.begin(), x.end()) - x.begin();
  }

  void run();
  /** \brief Complete the parallel split */
  void extract();
  /** \brief Get info */
  size_t input_size() const;
  /** \brief Get info */
  size_t output_size() const;
};

/** \brief Parallel operator
    \details Operator to evaluate a parallelized computational graph
*/
struct ParalOp : global::DynamicOperator<-1, -1> {
  static const bool have_input_size_output_size = true;
  /** \brief Computational graphs by thread */
  std::vector<global> vglob;
  /** \brief Indices to identify independent variable subset used by thread */
  std::vector<std::vector<Index> > inv_idx;
  /** \brief Indices to identify dependent variable subset calculated by thread
   */
  std::vector<std::vector<Index> > dep_idx;

  Index n, m;
  Index input_size() const;
  Index output_size() const;
  ParalOp(const autopar &ap);

  template <class T>
  void reverse(ReverseArgs<T> &args) {
    bool parallel_operator_used_with_valid_type = false;
    ASSERT(parallel_operator_used_with_valid_type);
  }
  static const bool add_forward_replay_copy = true;
  template <class T>
  void forward(ForwardArgs<T> &args) {
    bool parallel_operator_used_with_valid_type = false;
    ASSERT(parallel_operator_used_with_valid_type);
  }

  void forward(ForwardArgs<Scalar> &args);
  void reverse(ReverseArgs<Scalar> &args);
  const char *op_name();
  void print(global::print_config cfg);
};

std::vector<Index> get_likely_expression_duplicates(
    const global &glob, std::vector<Index> inv_remap);

/** \brief Test if all operators in the stack allow input remapping

    I.e. test if `op_info.allow_remap = true` for all operators.
*/
bool all_allow_remap(const global &glob);

/** \brief Forbid remappings if not consecutive */
template <class T>
struct forbid_remap {
  T &remap;
  forbid_remap(T &remap) : remap(remap) {}
  void operator()(Index a, Index b) {
    bool ok = true;
    for (Index i = a + 1; i <= b; i++) {
      ok &= (remap[i] - remap[i - 1] == 1);
    }
    if (ok) {
      return;
    } else {
      for (Index i = a; i <= b; i++) {
        remap[i] = i;
      }
    }
  }
};

/** \brief Remap identical sub-expressions

    \details

    ### Description
    Recall that all variables are stored sequentially on the
    tape and that each variable can be thought of as an 'expression'.
    The purpuse of the present function is to find out for any given
    variable whether it has already been calculated and, if so, find
    its first occurance. Finally, having constructed such a variable
    remapping table, we apply the remapping to all operator *inputs*,
    and return.

    This routine is thus only useful when followed by a call to the
    method `global::eliminate`, which will remove all redundant
    variables.

    ### Algorithm
    The algorithm essentially consists of two forward passes:

    1. **Hash step** Calculate a table of *likely* valid remappings
       using `global::hash_sweep` and `radix::first_occurance`. We
       call the resulting vector 'remap'. It satisfies `remap[i] <= i`
       with equality signifying that the variable `i` must be kept as
       is (which is always valid).

    2. **Proof step** Assume by induction that `remap[j]` is valid for
       all j strictly less than i. We must decide if `remap[i]` is
       valid, that is if the expressions `remap[i]` and `i` are
       identical. The two expressions are identical if they are result
       of the same operator *and* if their inputs are identical
       expressions. Because these inputs have index smaller than `i`
       we know that `remap` is valid for them. In other words we
       simply check that `remap[inputs(remap[i])]` is the same as
       `remap[inputs(i)]`. If this is the case we have proved that the
       expressions are equal and can accept the remapping. Otherwise
       we reject the remapping by setting `remap[i]=i`. In any case
       `remap[i]` now contains a valid remapping which completes the
       induction step.

    ### Applying the remap
    To summarize the above algorithm, it gives as output a vector
    `remap` such that expressions `i` and `remap[i]` are identical. We
    can thus apply the remap to all operator inputs and get an
    equivalent computational graph.

    However, special attention must be payed to operators with
    pointer inputs because they assume a contiguous memory layout.
    If an operator assumes a contiguous memory layout of the variables
    `a:b` we must require that the remapped variables `remap[a:b]` are
    also contiguous. This is done as a post rejection step by setting
    `remap[a:b] := a:b` for invalid remappings.

    \param glob Function object to be modified
    \param all_allow_remap Skip extra check
*/
std::vector<Index> remap_identical_sub_expressions(
    global &glob, std::vector<Index> inv_remap);

void remap_identical_sub_expressions(global &glob);

std::vector<Position> inv_positions(global &glob);

/** \brief Reorder computational graph such that selected independent variables
   come last \param inv_idx Sorted vector of independent variables. \note
   Nothing is done if **any operators with pointer inputs are detected on the
   tape**. (FIXME: Not optimal)
*/
void reorder_graph(global &glob, std::vector<Index> inv_idx);

}  // namespace TMBad
#endif  // HAVE_GRAPH_TRANSFORM_HPP

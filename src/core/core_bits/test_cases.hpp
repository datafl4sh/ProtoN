/*
 *       /\        Guillaume Delay 2018,2019
 *      /__\       guillaume.delay@enpc.fr
 *     /_\/_\      École Nationale des Ponts et Chaussées - CERMICS
 *    /\    /\
 *   /__\  /__\    This is ProtoN, a library for fast Prototyping of
 *  /_\/_\/_\/_\   Numerical methods.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * If you use this code or parts of it for scientific publications, you
 * are required to cite it as following:
 *
 * Implementation of Discontinuous Skeletal methods on arbitrary-dimensional,
 * polytopal meshes using generic programming.
 * M. Cicuttin, D. A. Di Pietro, A. Ern.
 * Journal of Computational and Applied Mathematics.
 * DOI: 10.1016/j.cam.2017.09.017
 */

template<typename T, typename Function, typename Mesh>
class test_case
{
   public:
    Function level_set_;
    std::function<T(const typename Mesh::point_type&)> sol_fun;
    std::function<T(const typename Mesh::point_type&)> rhs_fun;
    std::function<T(const typename Mesh::point_type&)> bcs_fun;

    test_case(){}
    
    test_case(Function level_set__,
              std::function<T(const typename Mesh::point_type&)> sol_fun_,
              std::function<T(const typename Mesh::point_type&)> rhs_fun_,
              std::function<T(const typename Mesh::point_type&)> bcs_fun_)
        : level_set_(level_set__), sol_fun(sol_fun_), rhs_fun(rhs_fun_), bcs_fun(bcs_fun_)
        {}
};


/////////////////////////////  TESTS CASES FOR LAPLACIAN  ////////////////////////////
template<typename T>
struct params
{
    T kappa_1, kappa_2, eta;

    params() : kappa_1(1.0), kappa_2(1.0), eta(30.0) {}
    params(T kap1, T kap2, T et) : kappa_1(kap1), kappa_2(kap2), eta(et) {}
};


template<typename T, typename Function, typename Mesh>
class test_case_laplacian: public test_case<T, Function, Mesh>
{
   public:
    std::function<Eigen::Matrix<T, 1, 2>(const typename Mesh::point_type&)> sol_grad;
    std::function<T(const typename Mesh::point_type&)> dirichlet_jump;
    std::function<T(const typename Mesh::point_type&)> neumann_jump;

    struct params<T> parms;

    test_case_laplacian(){}
    
    test_case_laplacian(Function level_set__, params<T> parms_,
                        std::function<T(const typename Mesh::point_type&)> sol_fun_,
                        std::function<T(const typename Mesh::point_type&)> rhs_fun_,
                        std::function<T(const typename Mesh::point_type&)> bcs_fun_,
                        std::function<Eigen::Matrix<T, 1, 2>
                        (const typename Mesh::point_type&)> sol_grad_,
                        std::function<T(const typename Mesh::point_type&)> dirichlet_jump_,
                        std::function<T(const typename Mesh::point_type&)> neumann_jump_)
    : test_case<T, Function, Mesh>(level_set__, sol_fun_, rhs_fun_, bcs_fun_),
        parms(parms_), sol_grad(sol_grad_), dirichlet_jump(dirichlet_jump_),
        neumann_jump(neumann_jump_)
        {}
};

///// test_case_laplacian_sin_sin
// exact solution : sin(\pi x) * sin(\pi y) in the whole domain
// \kappa_1 = \kappa_2 = 1
template<typename T, typename Function, typename Mesh>
class test_case_laplacian_sin_sin: public test_case_laplacian<T, Function, Mesh>
{
   public:
    test_case_laplacian_sin_sin(Function level_set__)
        : test_case_laplacian<T, Function, Mesh>
        (level_set__, params<T>(),
         [](const typename Mesh::point_type& pt) -> T { // sol
            return std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());},
         [](const typename Mesh::point_type& pt) -> T { // rhs
             return 2.0 * M_PI * M_PI * std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());},
         [&](const typename Mesh::point_type& pt) -> T { // bcs
             return std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());},
         [](const typename Mesh::point_type& pt) -> auto { // grad
             Matrix<T, 1, 2> ret;
             ret(0) = M_PI * std::cos(M_PI*pt.x()) * std::sin(M_PI*pt.y());
             ret(1) = M_PI * std::sin(M_PI*pt.x()) * std::cos(M_PI*pt.y());
             return ret;},
         [](const typename Mesh::point_type& pt) -> T {/* Dir */ return 0.0;},
         [](const typename Mesh::point_type& pt) -> T {/* Neu */ return 0.0;})
        {}
};

template<typename Mesh, typename Function>
auto make_test_case_laplacian_sin_sin(const Mesh& msh, Function level_set_function)
{
    return test_case_laplacian_sin_sin<typename Mesh::coordinate_type, Function, Mesh>(level_set_function);
}



///// test_case_laplacian_exp_cos
// exact solution : exp(x) * cos(y) in the whole domain
// \kappa_1 = \kappa_2 = 1
template<typename T, typename Function, typename Mesh>
class test_case_laplacian_exp_cos: public test_case_laplacian<T, Function, Mesh>
{
   public:
    test_case_laplacian_exp_cos(Function level_set__)
        : test_case_laplacian<T, Function, Mesh>
        (level_set__, params<T>(),
         [](const typename Mesh::point_type& pt) -> T { /* sol */
            return exp(pt.x()) * std::cos(pt.y());},
         [](const typename Mesh::point_type& pt) -> T { /* rhs */ return 0.0;},
         [&](const typename Mesh::point_type& pt) -> T { // bcs
             return exp(pt.x()) * std::cos(pt.y());},
         [](const typename Mesh::point_type& pt) -> auto { // grad
             Matrix<T, 1, 2> ret;             
             ret(0) = exp(pt.x()) * std::cos(pt.y());
             ret(1) = - exp(pt.x()) * std::sin(pt.y());
             return ret;},
         [](const typename Mesh::point_type& pt) -> T {/* Dir */ return 0.0;},
         [](const typename Mesh::point_type& pt) -> T {/* Neu */ return 0.0;})
        {}
};

template<typename Mesh, typename Function>
auto make_test_case_laplacian_exp_cos(const Mesh& msh, Function level_set_function)
{
    return test_case_laplacian_exp_cos<typename Mesh::coordinate_type, Function, Mesh>(level_set_function);
}

///// test_case_laplacian_jumps_1
// exact solution : sin(\pi x) sin(\pi y)  in \Omega_1
//                  exp(x) * cos(y)        in \Omega_2
// \kappa_1 = \kappa_2 = 1
template<typename T, typename Function, typename Mesh>
class test_case_laplacian_jumps_1: public test_case_laplacian<T, Function, Mesh>
{
   public:
    test_case_laplacian_jumps_1(Function level_set__)
        : test_case_laplacian<T, Function, Mesh>
        (level_set__, params<T>(),
         [level_set__](const typename Mesh::point_type& pt) -> T { /* sol */
            if(level_set__(pt) < 0)
                return std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());
            else return exp(pt.x()) * std::cos(pt.y());},
         [level_set__](const typename Mesh::point_type& pt) -> T { /* rhs */
             if(level_set__(pt) < 0)
                return 2.0 * M_PI * M_PI * std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());
            else return 0.0;},
         [level_set__](const typename Mesh::point_type& pt) -> T { // bcs
             if(level_set__(pt) < 0)
                 return std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());
             else return exp(pt.x()) * std::cos(pt.y());},
         [level_set__](const typename Mesh::point_type& pt) -> auto { // grad
             Matrix<T, 1, 2> ret;
             if(level_set__(pt) < 0)
             {
                 ret(0) = M_PI * std::cos(M_PI*pt.x()) * std::sin(M_PI*pt.y());
                 ret(1) = M_PI * std::sin(M_PI*pt.x()) * std::cos(M_PI*pt.y());
                 return ret;
             }
             else {
                 ret(0) = exp(pt.x()) * std::cos(pt.y());
                 ret(1) = - exp(pt.x()) * std::sin(pt.y());
                 return ret;}},
         [](const typename Mesh::point_type& pt) -> T {/* Dir */
             return std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y()) - exp(pt.x()) * std::cos(pt.y());},
         [level_set__](const typename Mesh::point_type& pt) -> T {/* Neu */
             Matrix<T, 1, 2> normal = level_set__.normal(pt);
             return (M_PI * std::cos(M_PI*pt.x()) * std::sin(M_PI*pt.y()) - exp(pt.x()) * std::cos(pt.y())) * normal(0) + ( M_PI * std::sin(M_PI*pt.x()) * std::cos(M_PI*pt.y()) + exp(pt.x()) * std::sin(pt.y()) ) * normal(1);})
        {}
};

template<typename Mesh, typename Function>
auto make_test_case_laplacian_jumps_1(const Mesh& msh, Function level_set_function)
{
    return test_case_laplacian_jumps_1<typename Mesh::coordinate_type, Function, Mesh>(level_set_function);
}


///// test_case_laplacian_jumps_2
// exact solution : exp(x) * cos(y)        in \Omega_1
//                  sin(\pi x) sin(\pi y)  in \Omega_2
// \kappa_1 = \kappa_2 = 1
template<typename T, typename Function, typename Mesh>
class test_case_laplacian_jumps_2: public test_case_laplacian<T, Function, Mesh>
{
   public:
    test_case_laplacian_jumps_2(Function level_set__)
        : test_case_laplacian<T, Function, Mesh>
        (level_set__, params<T>(),
         [level_set__](const typename Mesh::point_type& pt) -> T { /* sol */
            if(level_set__(pt) > 0)
                return std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());
            else return exp(pt.x()) * std::cos(pt.y());},
         [level_set__](const typename Mesh::point_type& pt) -> T { /* rhs */
             if(level_set__(pt) > 0)
                return 2.0 * M_PI * M_PI * std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());
            else return 0.0;},
         [level_set__](const typename Mesh::point_type& pt) -> T { // bcs
             if(level_set__(pt) > 0)
                 return std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y());
             else return exp(pt.x()) * std::cos(pt.y());},
         [level_set__](const typename Mesh::point_type& pt) -> auto { // grad
             Matrix<T, 1, 2> ret;
             if(level_set__(pt) > 0)
             {
                 ret(0) = M_PI * std::cos(M_PI*pt.x()) * std::sin(M_PI*pt.y());
                 ret(1) = M_PI * std::sin(M_PI*pt.x()) * std::cos(M_PI*pt.y());
                 return ret;
             }
             else {
                 ret(0) = exp(pt.x()) * std::cos(pt.y());
                 ret(1) = - exp(pt.x()) * std::sin(pt.y());
                 return ret;}},
         [](const typename Mesh::point_type& pt) -> T {/* Dir */
             return - std::sin(M_PI*pt.x()) * std::sin(M_PI*pt.y()) + exp(pt.x()) * std::cos(pt.y());},
         [level_set__](const typename Mesh::point_type& pt) -> T {/* Neu */
             Matrix<T, 1, 2> normal = level_set__.normal(pt);
             return -(M_PI * std::cos(M_PI*pt.x()) * std::sin(M_PI*pt.y()) - exp(pt.x()) * std::cos(pt.y())) * normal(0) - ( M_PI * std::sin(M_PI*pt.x()) * std::cos(M_PI*pt.y()) + exp(pt.x()) * std::sin(pt.y()) ) * normal(1);})
        {}
};

template<typename Mesh, typename Function>
auto make_test_case_laplacian_jumps_2(const Mesh& msh, Function level_set_function)
{
    return test_case_laplacian_jumps_2<typename Mesh::coordinate_type, Function, Mesh>(level_set_function);
}



///// test_case_laplacian_contrast_2
// !! available for circle_level_set only !!
// circle : radius = R, center = (a,b)
// exact solution : r^2 / \kappa_1 in \Omega_1
//                  (r^2 - R^2) / \kappa_2 + R^2 / \kappa_1 in \Omega_2
// \kappa_1 and \kappa_2 : parameters to choose (in parms_)
template<typename T, typename Mesh>
class test_case_laplacian_contrast_2: public test_case_laplacian<T, circle_level_set<T>, Mesh>
{
   public:
    test_case_laplacian_contrast_2(T R, T a, T b, params<T> parms_)
        : test_case_laplacian<T, circle_level_set<T>, Mesh>
        (circle_level_set<T>(R, a, b), parms_,
         [R, a, b, parms_](const typename Mesh::point_type& pt) -> T { /* sol */
            T r2 = (pt.x() - a) * (pt.x() - a) + (pt.y() - b) * (pt.y() - b);
            if(r2 < R*R) {return r2 / parms_.kappa_1;}
            else {return (r2 - R*R) / parms_.kappa_2
                    + R*R / parms_.kappa_1;} },
         [](const typename Mesh::point_type& pt) -> T { /* rhs */ return -4.0;},
         [R, a, b, parms_](const typename Mesh::point_type& pt) -> T { // bcs
            T r2 = (pt.x() - a) * (pt.x() - a) + (pt.y() - b) * (pt.y() - b);
            if (r2 < R*R) {return r2 / parms_.kappa_1;}
            else {return (r2 - R*R) / parms_.kappa_2
                    + R*R / parms_.kappa_1;} },
         [R, a, b, parms_](const typename Mesh::point_type& pt) -> auto { // grad    
             Matrix<T, 1, 2> ret;
             T r2 = (pt.x() - a) * (pt.x() - a) + (pt.y() - b) * (pt.y() - b);
             if (r2 < R*R)
             {
                 ret(0) = 2 * ( pt.x() - a ) / parms_.kappa_1 ;
                 ret(1) = 2 * ( pt.y() - b ) / parms_.kappa_1 ;
             }
             else
             {
                 ret(0) = 2 * ( pt.x() - a ) / parms_.kappa_2 ;
                 ret(1) = 2 * ( pt.y() - b ) / parms_.kappa_2 ;
             }
             return ret;},
         [](const typename Mesh::point_type& pt) -> T {/* Dir */ return 0.0;},
         [](const typename Mesh::point_type& pt) -> T {/* Neu */ return 0.0;})
        {}
};


template<typename Mesh>
auto make_test_case_laplacian_contrast_2(const Mesh& msh, circle_level_set<typename Mesh::coordinate_type> LS, params<typename Mesh::coordinate_type> parms)
{
    return test_case_laplacian_contrast_2<typename Mesh::coordinate_type, Mesh>(LS.radius, LS.alpha, LS.beta, parms);
}

///// test_case_laplacian_contrast_6
// !! available for circle_level_set only !!
// circle : radius = R, center = (a,b)
// exact solution : r^6 / \kappa_1 in \Omega_1
//                  (r^6 - R^6) / \kappa_2 + R^6 / \kappa_1 in \Omega_2
// \kappa_1 and \kappa_2 : parameters to choose (in parms_)
template<typename T, typename Mesh>
class test_case_laplacian_contrast_6: public test_case_laplacian<T, circle_level_set<T>, Mesh>
{
   public:
    test_case_laplacian_contrast_6(T R, T a, T b, params<T> parms_)
        : test_case_laplacian<T, circle_level_set<T>, Mesh>
        (circle_level_set<T>(R, a, b), parms_,
         [R, a, b, parms_](const typename Mesh::point_type& pt) -> T { /* sol */
            T r2 = (pt.x() - a) * (pt.x() - a) + (pt.y() - b) * (pt.y() - b);
            if(r2 < R*R) {return r2*r2*r2 / parms_.kappa_1;}
            else {return (r2*r2*r2) / parms_.kappa_2
                    + (R*R*R*R*R*R) * (1.0 / parms_.kappa_1 - 1.0 / parms_.kappa_2);} },
         [a, b](const typename Mesh::point_type& pt) -> T { /* rhs */
             T r2 = (pt.x() - a) * (pt.x() - a) + (pt.y() - b) * (pt.y() - b);
             return -36.0 * r2 * r2;},
         [R, a, b, parms_](const typename Mesh::point_type& pt) -> T { // bcs
            T r2 = (pt.x() - a) * (pt.x() - a) + (pt.y() - b) * (pt.y() - b);
            if(r2 < R*R) {return r2*r2*r2 / parms_.kappa_1;}
            else {return (r2*r2*r2) / parms_.kappa_2
                    + (R*R*R*R*R*R) * (1.0 / parms_.kappa_1 - 1.0 / parms_.kappa_2);} },
         [R, a, b, parms_](const typename Mesh::point_type& pt) -> auto { // grad    
             Matrix<T, 1, 2> ret;
             T r2 = (pt.x() - a) * (pt.x() - a) + (pt.y() - b) * (pt.y() - b);
             if (r2 < R*R)
             {
                 ret(0) = 6 * r2 * r2 * ( pt.x() - a ) / parms_.kappa_1 ;
                 ret(1) = 6 * r2 * r2 * ( pt.y() - b ) / parms_.kappa_1 ;
             }
             else
             {
                 ret(0) = 6 * r2 * r2 * ( pt.x() - a ) / parms_.kappa_2 ;
                 ret(1) = 6 * r2 * r2 * ( pt.y() - b ) / parms_.kappa_2 ;
             }
             return ret;},
         [](const typename Mesh::point_type& pt) -> T {/* Dir */ return 0.0;},
         [](const typename Mesh::point_type& pt) -> T {/* Neu */ return 0.0;})
        {}
};


template<typename Mesh>
auto make_test_case_laplacian_contrast_6(const Mesh& msh, circle_level_set<typename Mesh::coordinate_type> LS, params<typename Mesh::coordinate_type> parms)
{
    return test_case_laplacian_contrast_6<typename Mesh::coordinate_type, Mesh>(LS.radius, LS.alpha, LS.beta, parms);
}
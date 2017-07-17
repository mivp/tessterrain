//
//  Utils.hpp
//  tessterrain
//
//  Created by Toan Nguyen on 17/7/17.
//
//

#ifndef Utils_hpp
#define Utils_hpp

namespace tessterrain {

#define DOT3(v, w) ((v)[0] * (w)[0] + \
(v)[1] * (w)[1] + \
(v)[2] * (w)[2])

class Utils {
public:
    static unsigned int getTime();
    static int testPlane(const float V[4], const float b[6]);
    static int testFrustum(float V[6][4], const float b[6]);
    static void getFrustum(float V[6][4], const float X[16]);
};
    
}; // namespace

#endif /* Utils_hpp */

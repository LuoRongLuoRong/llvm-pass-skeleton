#include <iostream>
#include <vector>

using namespace std;

class Result {
private:    
    int a = 1;
public:
    vector<int> generate(vector<int> basicVars) {
        int _idx = 0;
        int dLOS = basicVars[_idx++];
        int dInPwrLow = basicVars[_idx++];
        int dLOF = basicVars[_idx++];
        
        int cLOS = 0, cLOF = 0;
        // detailed code
        return {cLOS, cLOF};
    }
};

int main() {
    Result result;
    vector<int> vals(3);
    

    return 0;
}

/*
public class Result {
    final int ALM_NO = 0;
    final int ALM_YES = 1;
    final int ALM_LOF = 2;
    final int ALM_MAX = 3;
    final int ALM_LOS = 0;
    final int ALM_INPWR_LOW = 1;

    public int[] generate(int[] basicVars) {
        int i = 0;
        int dLOS = basicVars[i++];
        int dInPwrLow = basicVars[i++];
        int dLOF = basicVars[i++];
        int type = basicVars[i++];
        int cLOS = 0, cLOF = 0;
        cLOS = match2int((0) );
        cLOF = match2int((1) );

        switch (type) {
            case ALM_LOS:
                cLOS = match2int((((0) )));

                if (match2bool(match2bool(dInPwrLow == 0))) {
                    cLOS = match2int((dLOS ));

                    if (match2bool(match2bool(! false ) && match2bool( (1) == ((dLOS ))))) {
                        cLOF = match2int((0) );

                    }
                    else {

                    }
                }
                else {
                    cLOS = match2int((dLOF ));

                    if (match2bool(match2bool(! false ) && match2bool( (1) == ((dLOF ))))) {
                        cLOF = match2int((0) );

                    }
                    else {

                    }
                }
                break;
            case ALM_INPWR_LOW:
                cLOS = match2int((dInPwrLow ));

                if (match2bool(match2bool(! false ) && match2bool( (1) == ((dInPwrLow ))))) {
                    cLOF = match2int((0) );

                }
                else {

                }
                break;
            case ALM_LOF:
                cLOS = match2int(((dLOF ) | ( dInPwrLow & dLOS ) ));

                if (match2bool(match2bool(!false ) && match2bool( (1) == (((dLOF ) | ( dInPwrLow & dLOS ) ))))) {
                    cLOF = match2int((0) );

                }
                else {

                }
                break;
            default:
                cLOS = match2int((((0) )));

                if (match2bool(match2bool(! true ) && match2bool( (1) == ((((0) )))))) {
                    cLOF = match2int((0) );

                }
                else {

                }
                break;
        }
        return new int[]{cLOS, cLOF};
    }
}

*/

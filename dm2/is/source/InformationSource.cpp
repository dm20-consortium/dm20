#include "InformationSource.h"

using namespace std;

namespace IS {

    /**
     * コンストラクタ
     *
     * @author	Nagoya University
     * @date	2018/03/15
     *
     * @param	name	名称
     */

    InformationSource::InformationSource(string name)
    {
        sourcename = name;
    }

    string InformationSource::getName()
    {
        return sourcename;
    } 
}
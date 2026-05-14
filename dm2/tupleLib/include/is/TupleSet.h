#ifndef TUPLESET_H
#define TUPLESET_H

#include "is/Tuple.h"
#include "is/Schema.h"

#include <iostream>
#include <vector>

namespace IS {

	class ArrivalTupleListener;
    /**
     * 複数のタプルとスキーマ情報を扱うクラス
     *
     * @author	Nagoya University
     * @date	2018/03/14
     */

    class TupleSet {

    private:
        std::vector<Tuple> tuples;
        Schema schema;

    public:
        TupleSet();
        ~TupleSet();

		// タプルの発生起源タイプ
		enum OccurrenceType {
			master	= 0,	// master句によって生成されたデータ
			self	= 1,	// ロジックによって生成されたデータ(One-Shot Queryなど)
			pull	= 2		// requestDataによって下位から要求して生成されたデータ
		};
		struct Info
		{
			OccurrenceType type = OccurrenceType::self;
			long recvTime = 0;
			bool isRecursive = false;
			int orgAttributeSize = 0;
			string orgTableName = "";
		};

		Info info;
		const ArrivalTupleListener* fromOperator = NULL;

        Schema getSchema() const;
		Schema& getSchemaRef();
        void SetSchema(Schema in);

        Tuple& getTuple(unsigned int idx);
		void addTuple(const Tuple &tuple);
		void setTuple(const int idx, const Tuple &tuple);
		void deleteTuple(unsigned int idx);
		int size() const;
		void resize(const int size);
		vector<Tuple> getTuples() const;
		vector<Tuple>& getTupleVec();
		void clear();
		void clearTuples();
		void updateTimestampByIdx(int rowIdx, int colIdx, long timestamp);
		void updateSchemaColumnName(const string &from, const string &to);
		void removeColumn(const int colIdx);
		void removeColumnOverOrgSize();
		void removeColumn(const string &colName);
		void reverse();
		void sortByCost();
		void debugKey();
    };
}
#endif  //TUPLESET_H

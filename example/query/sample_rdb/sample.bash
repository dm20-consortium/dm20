#!/bin/bash
COI_SQL_PATH="coi"
DB_NAME="dm2_db"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SETTING_PY=${SCRIPT_DIR}/dm2db_settings.py

echo "***** 03. DM2 SAMPLE DATA INSERT *****"
coi_table_items=(
    st_edge
    st_face
    st_node
    attribute_composition
    attribute_group
    cmp_feat_part_attr
    feature_attr
    feature_category
    feature_class_code
    pf_area_feature
    pf_area_topo_prim
    pf_comp_feat_part
    pf_comp_feature
    pf_line_feature
    pf_line_topo_prim
    pf_point_feature
    relation_feat_attr
    relation_role
    relation_type_code
    relationship
    relationship_attr
    relationship_feat
);

#dm2スキーマの作成
python3 ${SETTING_PY} ${DB_NAME} "select * from pg_namespace where nspname = 'dm2'"
if [ $? -eq 0 ]; then
        python3 ${SETTING_PY} ${DB_NAME} "CREATE SCHEMA dm2"
fi
#テーブルのCREATE
for coi_table_item in "${coi_table_items[@]}" ; do
        echo "[ ${coi_table_item} ]"
        python3 ${SETTING_PY} ${DB_NAME} "select * from pg_tables where tablename = '${coi_table_item}'"
        if [ $? -eq 0 ]; then
                python3 ${SETTING_PY} ${DB_NAME} ${COI_SQL_PATH}/${coi_table_item}.sql
        fi
done
#テーブルへサンプルデータをコピー
for coi_table_item in "${coi_table_items[@]}" ; do
        echo "[ ${coi_table_item} ]"
        python3 ${SETTING_PY} ${DB_NAME} "select * from ${coi_table_item} limit 1"
        if [ $? -eq 0 ]; then
                python3 ${SETTING_PY} ${DB_NAME} copy ${coi_table_item} ${COI_SQL_PATH}/${coi_table_item}.dump
        fi
done
# ST_FACEのインデックス作成
python3 ${SETTING_PY} ${DB_NAME} "select * from pg_indexes where tablename = 'st_face' and indexname = 'face_mbr'"
if [ $? -eq 0 ]; then
        python3 ${SETTING_PY} ${DB_NAME} "CREATE INDEX face_mbr ON st_face USING gist (mbr)"
fi

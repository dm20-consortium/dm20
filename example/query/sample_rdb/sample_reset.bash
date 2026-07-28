#!/bin/bash
coi_table_items=(
    attribute_composition
    attribute_group
    cmp_feat_part_attr
    feature_attr
    feature_category
    feature_class_code
    pf_area_topo_prim
    pf_area_feature
    pf_comp_feat_part
    pf_comp_feature
    pf_line_topo_prim
    pf_line_feature
    pf_point_feature
    relation_feat_attr
    relation_role
    relation_type_code
    relationship
    relationship_attr
    relationship_feat
    st_edge
    st_face
    st_node
)
for coi_table_item in "${coi_table_items[@]}" ; do
        echo "[ ${coi_table_item} ]"
        python3 dm2db_settings.py dm2_db "SET search_path = dm2;drop table ${coi_table_item}"
done

CREATE TABLE pf_comp_feat_part (
    comp_feat_category integer NOT NULL,
    comp_feature_id integer NOT NULL,
    feature_number integer NOT NULL,
    feature_category_num integer,
    feature_id integer,
    PRIMARY KEY (comp_feat_category, comp_feature_id, feature_number)
);
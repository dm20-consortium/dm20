CREATE TABLE pf_comp_feature (
    feat_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    feature_class_code character(4),
    from_feat_category integer,
    from_feature_id integer,
    to_feat_category integer,
    to_feature_id integer,
    PRIMARY KEY (feat_category_num, feature_id)
);
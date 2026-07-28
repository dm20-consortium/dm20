CREATE TABLE pf_line_feature (
    feat_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    feature_class_code character(4),
    end_elevation integer,
    from_feat_category integer,
    from_feat_id integer,
    to_feat_category integer,
    to_feat_id integer,
    PRIMARY KEY (feat_category_num, feature_id)
);
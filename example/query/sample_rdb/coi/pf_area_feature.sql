CREATE TABLE pf_area_feature (
    feat_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    feature_class_code character(4),
    PRIMARY KEY (feat_category_num, feature_id)
);
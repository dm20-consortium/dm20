CREATE TABLE cmp_feat_part_attr (
    feat_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    sequence_number integer,
    attribute_id integer NOT NULL,
    PRIMARY KEY (feat_category_num, feature_id, attribute_id)
);
CREATE TABLE pf_point_feature (
    feat_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    feature_class_code character(4),
    node_id integer,
    PRIMARY KEY (feat_category_num, feature_id),
    FOREIGN KEY (node_id) REFERENCES st_node(node_id)
);
CREATE TABLE feature_attr (
    feature_category_num integer NOT NULL,
    feature_id integer NOT NULL,
    attribute_id integer NOT NULL,
    PRIMARY KEY (feature_category_num, feature_id, attribute_id)
);
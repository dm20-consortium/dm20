CREATE TABLE relationship_feat (
    relationship_id integer NOT NULL,
    role_number integer NOT NULL,
    feature_number integer NOT NULL,
    feat_category_num integer,
    feature_id integer,
    PRIMARY KEY (relationship_id, role_number, feature_number)
);
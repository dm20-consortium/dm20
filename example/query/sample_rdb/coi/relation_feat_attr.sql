CREATE TABLE relation_feat_attr (
    relationship_id integer NOT NULL,
    role_number integer NOT NULL,
    feature_number integer NOT NULL,
    attribute_id integer NOT NULL,
    PRIMARY KEY (relationship_id, role_number, feature_number, attribute_id)
);
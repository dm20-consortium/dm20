CREATE TABLE attribute_composition (
    attribute_id integer NOT NULL,
    scope_level integer NOT NULL,
    composition_number integer NOT NULL,
    sequence_number integer NOT NULL,
    parent_scope_level integer,
    parent_comp_number integer,
    parent_seq_number integer,
    av_attr_value character varying,
    av_type_code character(2),
    PRIMARY KEY (attribute_id, scope_level, composition_number, sequence_number)
);
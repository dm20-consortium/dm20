CREATE TABLE relation_role (
    rel_type_code character(4) NOT NULL,
    role_number integer NOT NULL,
    role_name character varying(40),
    repeatable boolean,
    mandatory boolean,
    PRIMARY KEY (rel_type_code, role_number)
);
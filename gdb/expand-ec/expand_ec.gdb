set pagination off
set print pretty
set print repeats 0
set logging on ./expand_ec.log

b ec_expand_do
commands
        bt
        c
end

b ec_expand_data
commands
        bt
        c
end

b __ec_expand_data
commands
        bt
        c
end

b ec_expand_entry
commands
        bt
        c
end

b ec_expand_full_sweep
commands
        bt
        c
end

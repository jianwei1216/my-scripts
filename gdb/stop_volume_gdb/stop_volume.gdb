set pagination off
set print pretty
set logging on ./stop.log

b digioceand_set_brick_status if status==0
commands
    p brickinfo->hostname
    bt
    c
end

b digioceand_set_brick_status if status==1
commands
    p brickinfo->hostname
    bt
    c
end

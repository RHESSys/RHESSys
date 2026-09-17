#devtools::install_github("RHESSys/RHESSysIOinR")

# load libraries
library(tidyverse)
library(RHESSysIOinR)
library(ggpubr)
library(lubridate)
load("W8TestCase.RData")

# name <- "TestCase"

# Basic RHESSys inputs
rhessys_input <- IOin_rhessys_input(
  version = "../rhessys/rhessys7.5",
  world_file = "worldfiles/w8TC.world",
  # world_hdr_prefix = paste0("hdr_",name),
  # world_hdr_prefix = NULL, 
  flowtable = "flowtables/w8TC.flow",
  tec_file = "tecfiles/tec.test",
  start = "1988 10 1 1",
  end = "2000 10 1 1",
  output_prefix = NULL,
  output_folder = NULL,
  commandline_options = "-g -s 0.355794 651.390265 -sv 0.355794 651.390265 -svalt 1.083102 1.193924 -gw 0.116316 0.916922"
)

header <- "worldfiles/w8TC.hdr"
# hdr <- read.table(header)
offile <- "tecfiles/testing_filter.yml"

rh_cmd <- run_rhessys_single(
  input_rhessys = rhessys_input,
  hdr_files = "worldfiles/w8TC.hdr",  
  output_filter = "tecfiles/testing_filter.yml"
)

# ========================= Output =========================

### Read in the test results and plot the water balance results
test <- read.table("out/test_basin_daily.csv", header = T, sep = ",")
test <- add_dates(test)
test <- watbal_basin_of(test)

# Add other variables present in base data frame but not in test output, and used in subsequent tests and plots
test <- mutate(
  test,
  psn = cs.net_psn,
  gpsn = cdf.psn_to_cpool,
  plantc = cs.total_plantc,
  plant_resp = cdf.total_mr + cdf.total_gr
)

ggplot(test, aes(date, watbal)) + geom_point()

# your results are in red
ggplot(base, aes(date, watbal)) +
  geom_point() +
  geom_point(data = test, aes(date, watbal), col = "red", alpha = 0.4)
# note base needs to be updated

# many variables in base have units changed so need to be updated
a <- ggplot(base, aes(date, epv.height)) +
  geom_point() +
  geom_point(data = test, aes(date, epv.height), col = "red", alpha = 0.4)
b <- ggplot(base, aes(date, epv.proj_lai)) +
  geom_point() +
  geom_point(data = test, aes(date, epv.proj_lai), col = "red", alpha = 0.4)
c <- ggplot(base, aes(date, sat_deficit)) +
  geom_point() +
  geom_point(data = test, aes(date, sat_deficit), col = "red", alpha = 0.4)
d <- ggplot(base, aes(date, streamflow)) +
  geom_point() +
  geom_point(
    data = test,
    aes(date, streamflow * 1000),
    col = "red",
    alpha = 0.4
  )

ggarrange(a, b, c, d)

d <- ggplot(base, aes(date, litter_cs.totalc)) +
  geom_point() +
  geom_point(data = test, aes(date, litter_cs.totalc), col = "red", alpha = 0.4)
e <- ggplot(base, aes(date, soil_cs.totalc)) +
  geom_point() +
  geom_point(data = test, aes(date, soil_cs.totalc), col = "red", alpha = 0.4)



### ------- I'm confussed what the goal of this section is supposed to be - I've made an alternative version
cor.test(base$obs_m, base$model_Q)

test$model_Q = test$streamflow + test$gw.Qout
cor.test(test$streamflow, test$model_Q)

# error/bias
test_err = (test$model_Q - test$streamflow) %>% mean(na.rm=TRUE)
base_err = (base$model_Q - base$obs_m) %>% mean(na.rm=TRUE)

test_err/mean(test$streamflow, na.rm=TRUE)*100
base_err/mean(base$obs_m, na.rm=TRUE)*100

#  --------- alternative using the base obs Q throughout -----------
# Comparison with observed streamflow
test$model_Q <- test$streamflow + test$gw.Qout

cor.test(base$obs_m, base$model_Q)
cor.test(base$obs_m, test$model_Q)

# error/bias
test_err <- (test$model_Q - base$obs_m) %>% mean(na.rm = TRUE)
base_err <- (base$model_Q - base$obs_m) %>% mean(na.rm = TRUE)
test_err / mean(base$obs_m, na.rm = TRUE) * 100
base_err / mean(base$obs_m, na.rm = TRUE) * 100



### Difference between the base and the test scenarios
# this doesn't include cpool currently
base.mean <- base %>%
  select(streamflow, cs.net_psn, cs.dead_crootc, cs.live_crootc, cs.frootc, cs.live_stemc, cs.dead_stemc, cs.leafc, transpiration_sat_zone, transpiration_unsat_zone, evaporation, evaporation_surf, cs.totalc) %>%
  summarize_if(is.numeric, mean)

test.mean <- test %>%
  select(streamflow, cs.net_psn, cs.dead_crootc, cs.live_crootc, cs.frootc, cs.live_stemc, cs.dead_stemc, cs.leafc, transpiration_sat_zone, transpiration_unsat_zone, evaporation, evaporation_surf, cs.totalc) %>%
  summarize_if(is.numeric, mean)

percent_diff_mean <- ((test.mean - base.mean) / base.mean) * 100

percent_diff_mean

base.grow.mean <- base %>%
  select(
    streamflow_NO3,
    ndf.sminn_to_nitrate,
    ndf.denitrif,
    cdf.total_gr,
    cdf.psn_to_cpool,
    soil_cs.totalc,
    litter_cs.totalc
  ) %>%
  summarize_if(is.numeric, mean)
test.grow.mean <- test %>%
  select(
    streamflow_NO3,
    ndf.sminn_to_nitrate,
    ndf.denitrif,
    cdf.total_gr,
    cdf.psn_to_cpool,
    soil_cs.totalc,
    litter_cs.totalc
  ) %>%
  summarize_if(is.numeric, mean)

percent_gdiff_mean <- ((test.grow.mean - base.grow.mean) / base.grow.mean) * 100
percent_gdiff_mean


## Time Series Plots

####  Time Series

# baseline
tmp <- base %>%
  select(
    date,
    streamflow,
    epv.proj_lai,
    snowpack.water_equivalent_depth,
    cs.net_psn,
    sat_deficit,
    rz_storage,
    transpiration_sat_zone,
    transpiration_unsat_zone,
    evaporation,
    evaporation_surf,
    epv.height
  )
tmp2 <- base %>%
  select(
    cs.totalc,
    streamflow_NO3,
    litter_cs.totalc,
    soil_cs.totalc,
    cdf.psn_to_cpool,
    cdf.total_gr,
    ndf.sminn_to_nitrate,
    ndf.denitrif
  )
tmpb <- cbind.data.frame(tmp, tmp2)
tmpb$new <- FALSE

# new results
tmp <- test %>%
  select(
    date,
    streamflow,
    epv.proj_lai,
    snowpack.water_equivalent_depth,
    cs.net_psn,
    sat_deficit,
    rz_storage,
    transpiration_sat_zone,
    transpiration_unsat_zone,
    evaporation,
    evaporation_surf,
    epv.height
  )
tmp2 <- test %>%
  select(
    cs.totalc,
    streamflow_NO3,
    litter_cs.totalc,
    soil_cs.totalc,
    cdf.psn_to_cpool,
    cdf.total_gr,
    ndf.sminn_to_nitrate,
    ndf.denitrif
  )
tmpt <- cbind.data.frame(tmp, tmp2)
tmpt$new <- TRUE

tmpa2 <- inner_join(tmpb, tmpt, by = c("date"), suffix = c(".base", ".test"))


# if any of these values are large > +- 4e-6 then you should understand the reason
res <- data.frame(matrix(ncol = 0, nrow = 2))

res$streamflow <- c(
  max(tmpa2$streamflow.test - tmpa2$streamflow.base),
  min(tmpa2$streamflow.test - tmpa2$streamflow.base)
)

res$snowpack <- c(
  max(
    tmpa2$snowpack.water_equivalent_depth.test -
      tmpa2$snowpack.water_equivalent_depth.base
  ),
  min(
    tmpa2$snowpack.water_equivalent_depth.test -
      tmpa2$snowpack.water_equivalent_depth.base
  )
)

res$evap <- c(
  max(tmpa2$evaporation.test - tmpa2$evaporation.base),
  min(tmpa2$evaporation.test - tmpa2$evaporation.base)
)

res$trans_sat <- c(
  max(tmpa2$transpiration_sat_zone.test - tmpa2$transpiration_sat_zone.base),
  min(tmpa2$transpiration_sat_zone.test - tmpa2$transpiration_sat_zone.base)
)
  
res$trans_unsat <- c(
  max(tmpa2$transpiration_unsat_zone.test - tmpa2$transpiration_unsat_zone.base),
  min(tmpa2$transpiration_unsat_zone.test - tmpa2$transpiration_unsat_zone.base)
)

res$sat_def <- c(
  max(tmpa2$sat_deficit.test - tmpa2$sat_deficit.base),
  min(tmpa2$sat_deficit.test - tmpa2$sat_deficit.base)
)

res$rz_storage <- c(
  max(tmpa2$rz_storage.test - tmpa2$rz_storage.base),
  min(tmpa2$rz_storage.test - tmpa2$rz_storage.base)
)

res$plantc <- c(
  max(tmpa2$cs.totalc.test - tmpa2$cs.totalc.base),
  min(tmpa2$cs.totalc.test - tmpa2$cs.totalc.base)
)


res$soilc <- c(
  max(tmpa2$soil_cs.totalc.test - tmpa2$soil_cs.totalc.base),
  min(tmpa2$soil_cs.totalc.test - tmpa2$soil_cs.totalc.base)
)


res$litrc <- c(
  max(tmpa2$litter_cs.totalc.test - tmpa2$litter_cs.totalc.base),
  min(tmpa2$litter_cs.totalc.test - tmpa2$litter_cs.totalc.base)
)

res$psn <- c(
  max(tmpa2$cs.net_psn.test - tmpa2$cs.net_psn.base),
  min(tmpa2$cs.net_psn.test - tmpa2$cs.net_psn.base)
)

res$streamflow_NO3 <- c(
  max(tmpa2$streamflow_NO3.test - tmpa2$streamflow_NO3.base),
  min(tmpa2$streamflow_NO3.test - tmpa2$streamflow_NO3.base)
)

res$height <- c(
  max(tmpa2$epv.height.test - tmpa2$epv.height.base),
  min(tmpa2$epv.height.test - tmpa2$epv.height.base)
)


res <- t(res)
colnames(res) <- c("min", "max")
res


# If you see big departures you may want to look at plots
tmpa <- rbind.data.frame(tmpb, tmpt)

a <- ggplot(tmpa, aes(date, streamflow, col = new)) +
  geom_line() +
  scale_y_continuous(trans = "log")
b <- ggplot(tmpa, aes(date, cs.totalc, col = new)) + geom_line()
c <- ggplot(tmpa, aes(date, snowpack.water_equivalent_depth, col = new)) +
  geom_line()
d <- ggplot(tmpa, aes(date, transpiration_sat_zone, col = new)) + geom_line()
e <- ggplot(tmpa, aes(date, transpiration_unsat_zone, col = new)) + geom_line()
f <- ggplot(tmpa, aes(date, epv.proj_lai, col = new)) + geom_line()
g <- ggplot(tmpa, aes(date, evaporation, col = new)) + geom_line()
h <- ggplot(tmpa, aes(date, litter_cs.totalc, col = new)) + geom_line()
i <- ggplot(tmpa, aes(date, cs.net_psn, col = new)) + geom_line()
j <- ggplot(tmpa, aes(date, sat_deficit, col = new)) + geom_line()
k <- ggplot(tmpa, aes(date, streamflow_NO3, col = new)) + geom_line()
l <- ggplot(tmpa, aes(date, soil_cs.totalc, col = new)) + geom_line()
m <- ggplot(tmpa, aes(date, epv.height, col = new)) + geom_line()

# to see any of the individual plots, just type the letter for that plot

# you may have to zoom to see everything here
ggarrange(a, b, c, d, e, f, ncol = 3, nrow = 2)
ggarrange(g, h, i, j, k, l, m, ncol = 3, nrow = 3)

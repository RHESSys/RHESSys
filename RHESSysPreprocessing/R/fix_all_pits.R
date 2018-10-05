# call fill_pit on all pits
#
# By Daniel Nash
# Edited by Will Burke
#
fix_all_pits <- function(flw, flw_struct, parallel) {

  # ---------- Find pits ----------
  num_patch <- length(flw)
  x <- c()
  hght <- c()
  for (i in 1:num_patch) {
    if (parallel) { # for parallel
      if (sum(flw[[i]]$Gamma_i) == 0 & flw[[i]]$Landtype == 0) { # pits are where no flow out, and not a stream
        x <- c(x, i) # vector of pit patch numbers
        hght <- c(hght, flw[[i]]$Centroidz) # vector of pit heights
      }
    } else if (!parallel) { # non parallel
      if (sum(flw[[i]]$Gamma_i) == 0) { # pit is no flow out, stream doesn't matter
        x <- c(x, i) # vector of pit patch numbers
        hght <- c(hght, flw[[i]]$Centroidz) # vector of pit heights
      }
    }
  }

  # ---------- Exit if no pits ----------
  if (parallel) { # since parallel already ignores stream patches, basin outlet won't be included
    if (length(x) == 0) { # no pits
      return(flw)
    }
    pits <- x[order(hght)] # pit patch numbers ordered by elevation
  } else if (!parallel) {
    if (length(x) == 1) { # no pits
      return(flw)
    }
    pits <- x[order(hght)] # pit patch numbers ordered by elevation
    pits <- pits[-1] # throw out lowest pit - this is the outlet
  }
  num_pits <- length(pits)

  # ---------- Fill pits ----------
  no_neighbors <- NULL # stores index of patches with no neighbors, corrected if parallel=TRUE

  print(paste("Filling", num_pits, "pits."), quote = F)
  pb <- txtProgressBar(min = 0, max = num_pits, style = 3)

  for (j in 1:num_pits) {

    setTxtProgressBar(pb, j) #add progress
    i <- pits[j]

    #if (sum(flw[[i]]$Gamma_i) == 0) { # if is redundant, but if the gammas are all 0, try to pit fill
    filled <- fill_pit(flw = flw, n_s = i, n_f = i, parallel = parallel)
    #}

    wh_ct = 0 # while loop counter
    history = NULL

    while (!is.list(filled) & parallel) { # if fill pit fails returns NA

      wh_ct = wh_ct + 1
      if(wh_ct > 10){stop("Something has probably gone wrong. Looped through 10 patches with no neighbors")}

      history = c(history,filled)
      #  - for debugging - this output gets in the way of the progress bar and is covered by a table at the end anyways
      #print(paste("Ended pit fill attempt unsuccessfully for patch:", flw[[i]]$PatchID, "hillslope:", flw[[i]]$HillID))

      # ---------- Correct for hillslopes without neighbors ----------
      no_neighbors <- c(no_neighbors, i) # index of patches with no neighbors in hillslope

      # find downslope patches in same hillslope exluding self
      hill_subset <- flw_struct[flw_struct$Hill == flw_struct[i, ]$Hill &
        flw_struct$Number != flw_struct[i, ]$Number &
        flw_struct$Centroidz <= flw_struct[i, ]$Centroidz, ]

      if (nrow(hill_subset) == 0) {# if there are other patches in the same hillslope
        stop("Something has gone very wrong - there are no downslope patches in same hillslope.")
      }
      dist <- sqrt(abs(hill_subset$Centroidx - flw_struct[i, ]$Centroidx)^2 +
                     abs(hill_subset$Centroidy - flw_struct[i, ]$Centroidy)^2) # dist to patches
      new_neighbor <- hill_subset[which.min(dist), ]$Number # closest of downslope within hillslope patches
      flw[[i]]$Neighbors <- c(flw[[i]]$Neighbors, new_neighbor)
      flw[[i]]$Gamma_i <- c(flw[[i]]$Gamma_i, 1)
      flw[[new_neighbor]]$Neighbors <- c(flw[[new_neighbor]]$Neighbors, i)
      flw[[new_neighbor]]$Gamma_i <- c(flw[[new_neighbor]]$Gamma_i, 0)
      new_gamma_tot <- sum(flw[[new_neighbor]]$Gamma_i)

      # if new neighbor is not a pit OR is a stream
      if (new_gamma_tot != 0 | flw[[new_neighbor]]$Landtype == 1) {
        flw[[new_neighbor]]$Gamma_i <- flw[[new_neighbor]]$Gamma_i/new_gamma_tot   # normalize new gamma's - this may be unecessary
        filled <- flw # to escape the while loop
      } else if (new_gamma_tot == 0 & flw[[new_neighbor]]$Landtype == 0) {
        # if new neighbor is a pit AND not a stream, loop through this again, starting with an attempt to normal pit fill
        # while is needed in case this fails and next pit also has no neighbors
        filled <- fill_pit(flw = flw, n_s = i, n_f = new_neighbor, parallel = parallel,history = history)
      }
    } # end while
    flw <- filled # end use of temporary var
  }
  close(pb)

  # print table of patches with no neighbors
  if (!is.null(no_neighbors)) {
    print("The following patches have no neighbors within their hillslopes and were routed to the closest within-hillslope patch.",
      quote = FALSE)
    print(flw_struct[no_neighbors, ])
  }

  return(flw)
}

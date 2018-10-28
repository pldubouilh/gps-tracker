package main

import (
	"log"
	"net/http"
)

func handler(w http.ResponseWriter, r *http.Request) {
	log.Println(r.UserAgent(), ">>", r.URL)
	w.WriteHeader(421)
	w.Write([]byte("ok"))
}

func main() {
	http.HandleFunc("/", handler)
	log.Fatal(http.ListenAndServe(":9980", nil))
}
